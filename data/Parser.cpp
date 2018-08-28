//
// Created by stephane on 19/07/18.
//

#include "Parser.h"
#include <boost/tokenizer.hpp>
#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <proj.h>
#include <fstream>
#include <mongoc.h>

// a custom double parser
double parse_double(const std::string &str) {
    bool is_negative = false;
    int i = 0;
    int state = 0;
    double div = 1.0;
    double val = 0.0;
    int int_part = 0;
    int dec_part = 0;

    while (i < str.size()) {
        switch (state) {
            case 0:
                if (str[i] == '-') {
                    is_negative = true;
                    state = 1;
                } else if (str[i] == '+') {
                    state = 1;
                } else if (str[i] == '.') {
                    state = 3;
                } else if (std::isdigit(str[i])) {
                    int_part = (int) (str[i] - '0');
                    state = 2;
                }
                break;
            case 1:
                if (std::isdigit(str[i])) {
                    int_part = (int) (str[i] - '0');
                    state = 2;
                } else if (str[i] == '.')
                    state = 3;
                break;
            case 2:
                if (std::isdigit(str[i])) {
                    int_part = 10 * int_part + (int) (str[i] - '0');
                } else if (str[i] == '.')
                    state = 3;
                else
                    state = 5;
                break;
            case 3:
                if (std::isdigit(str[i])) {
                    dec_part = (int) (str[i] - '0');
                    div *= 0.1;
                    state = 4;
                } else
                    state = 5;
                break;
            case 4:
                if (std::isdigit(str[i])) {
                    dec_part = 10 * dec_part + (int) (str[i] - '0');
                    div *= 0.1;
                } else
                    state = 5;
        }
        i++;
    }
    if (state == 5)
        throw std::invalid_argument("Malformed double constant");
    val = (double) (int_part) + (double) (dec_part) * div;
    if (is_negative)
        val = -val;
    return val;
}

Parser::Parser() {

}

void Parser::parse_directory(QString &dir) {
    fs::path path(dir.toStdString());
    // check that the argument is a directory
    if (std::filesystem::is_directory(path) == false)
        return;

    // enumerate directory entries
    int i = 0;
    for (auto &p: fs::directory_iterator(path)) {
        std::cout << p.path() << std::endl;
        parse_file(p.path());
        i++;
        if(i > 0)
            break;
    }
}

void Parser::parse_file(const fs::path &path) {
    //std::ofstream outfile;
    // connect to mongodb
    const char *uri_string = "mongodb://localhost:27017";
    mongoc_uri_t *uri;
    mongoc_client_t *client;
    mongoc_database_t *database;
    mongoc_collection_t *collection;
    //bson_oid_t oid;
    bson_t *document;
    bson_error_t error;
    uri = mongoc_uri_new_with_error (uri_string, &error);
    if (!uri) {
        fprintf (stderr,
                 "failed to parse URI: %s\n"
                 "error message:       %s\n",
                 uri_string,
                 error.message);
        return;
    }

    /*
     * Create a new client instance
     */
    client = mongoc_client_new_from_uri (uri);
    if (!client) {
        return;
    }

    /*
     * Register the application name so we can track it in the profile logs
     * on the server. This can also be done from the URI (see other examples).
     */
    mongoc_client_set_appname (client, "spdcomplexity");

    /*
     * Get a handle on the database "db_name" and collection "coll_name"
     */
    database = mongoc_client_get_database (client, "atcdatabase");
    collection = mongoc_client_get_collection (client, "atcdatabase", "plots");

    //outfile.open("res.dump");
    // check file suffix to apply decompression
    if (path.extension() == ".gz") {
        // try to open file
        std::ifstream in_file(path, std::ios_base::in | std::ios_base::binary);
        // create a filtering stream
        boost::iostreams::filtering_stream<boost::iostreams::input> filter_stream;
        filter_stream.push(boost::iostreams::gzip_decompressor());
        filter_stream.push(in_file);
        // read lines
        std::string line;
        int i = 0;
        boost::char_separator<char> sep("|");
        // intermediate storage
        int id;
        boost::posix_time::ptime time;
        struct tm tm_time;
        PJ_COORD cart, latlon;
        double vx, vy, vz;
        double heading, velocity;
        PJ *stereo = proj_create(0, "+proj=stere +lat_0=47.0 +lon_0=0.0 +ellps=WGS84");
        while (std::getline(filter_stream, line)) {
            if (i == 1)
                std::cout << line << std::endl;
            // split the line into fields
            boost::tokenizer<boost::char_separator<char> > tok(line, sep);
            auto it = tok.begin();
            id = std::stoi(*it);
            it++;
            time = boost::posix_time::time_from_string(*it);
            tm_time = boost::posix_time::to_tm(time);
            try {
                it++;
                cart.xyz.x = parse_double(*it) * 1852.0;
                it++;
                cart.xyz.y = parse_double(*it) * 1852.0;
                it++;
                cart.xyz.z = parse_double(*it) * 0.3048;
                latlon = proj_trans(stereo, PJ_DIRECTION::PJ_INV, cart);
                it++;
            } catch (std::invalid_argument &e) {
                std::cerr << "Invalid line # " << i << std::endl;
                std::cerr << line << std::endl;
            }
            velocity = parse_double(*it) * 0.5144444;
            it++;
            heading = parse_double(*it) * M_PI / 180.0;
            it++;
            vx = velocity * cos(heading);
            vy = velocity * sin(heading);
            vz = parse_double(*it) * 0.00508;
      //      outfile << id << '\t' << time << '\t' << " POINT(" << latlon.xyz.x;
      //      outfile << ' ' << latlon.xyz.y << ")" << '\t' << latlon.xyz.z;
      //      outfile << '\t' << vx << '\t' << vy << '\t' << vz << std::endl;
            // create bson document
            //bson_oid_init (&oid, NULL);
            document = BCON_NEW(
                    "id",BCON_INT32(id),
                    "t",BCON_DATE_TIME(mktime(&tm_time) * 1000),
                    "loc","{",
                    "type",BCON_UTF8("Point"),
                    "coordinates","[",
                    BCON_DOUBLE(latlon.xyz.x * 180.0 / M_PI),
                    BCON_DOUBLE(latlon.xyz.y * 180.0 / M_PI),
                    BCON_DOUBLE(latlon.xyz.z),
                    "]","}",
                    "speed","[",
                    BCON_DOUBLE(vx),
                    BCON_DOUBLE(vy),
                    BCON_DOUBLE(vz),
                    "]"
                    );
            // insert into database
            if (!mongoc_collection_insert_one (
                    collection, document, NULL, NULL, &error)) {
                fprintf (stderr, "%s\n", error.message);
                break;
            }
            i++;
            if(i % 100000 == 0) {
                std::cout << i << " lines " << std::endl;
            }

        }
        std::cout << i << " lines " << std::endl;
        // close database connection
        mongoc_collection_destroy (collection);
        mongoc_database_destroy (database);
        mongoc_uri_destroy (uri);
        mongoc_client_destroy (client);

       // outfile.close();
        proj_destroy(stereo);
    }
}


ParserException::ParserException(std::string const &msg) throw() : m_msg(msg) {

}

