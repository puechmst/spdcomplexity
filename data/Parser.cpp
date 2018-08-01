//
// Created by stephane on 19/07/18.
//

#include "Parser.h"
#include <boost/tokenizer.hpp>
#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <proj.h>


Parser::Parser() {

}

void Parser::parse_directory(QString &dir) {
    fs::path path(dir.toStdString());
    // check that the argument is a directory
    if(std::filesystem::is_directory(path) == false)
        return;

    // enumerate directory entries
    for(auto &p: fs::directory_iterator(path)) {
        std::cout << p.path() << std::endl;
        parse_file(p.path());
    }
}

void Parser::parse_file(const fs::path &path) {

    // check file suffix to apply decompression
    if(path.extension() == ".gz" ) {
        // try to open file
        std::ifstream in_file(path, std::ios_base::in | std::ios_base::binary);
        // create a filtering stream
        boost::iostreams::filtering_stream<boost::iostreams::input> filter_stream;
        filter_stream.push(boost::iostreams::gzip_decompressor());
        filter_stream.push(in_file);
        // read lines
        std::string line;
        int i= 0;
        boost::char_separator<char> sep("|");
        // intermediate storage
        int id;
        boost::posix_time::ptime time;
        PJ_COORD cart, latlon;
        double vx,vy,vz;
        double heading, velocity;
        PJ * stereo = proj_create(0, "+proj=stere +lat_0=47.0 +lon_0=0.0 +ellps=WGS84");
        while(std::getline(filter_stream, line)) {
            if (i <100 )
                std::cout << line << std::endl;
            // split the line into fields
            boost::tokenizer<boost::char_separator<char> > tok(line, sep);
            auto it= tok.begin();
            id = std::stoi(*it);
            it++;
            time= boost::posix_time::time_from_string(*it);
            it++;
            cart.xyz.x = std::stod(*it) * 1852.0;
            it++;
            cart.xyz.y = std::stod(*it) * 1852.0;
            it++;
            cart.xyz.z = std::stod(*it) * 0.3048;
            latlon = proj_trans(stereo, PJ_DIRECTION::PJ_INV, cart);
            it++;

            if(i < 10)
                std::cout << id << '\t' << latlon.lp.lam << '\t' << latlon.lp.phi <<std::endl;
            i++;
        }
        std::cout << i << " lines " << std::endl;
        proj_destroy(stereo);
    }
}


ParserException::ParserException(std::string const& msg) throw() : m_msg(msg) {

}

