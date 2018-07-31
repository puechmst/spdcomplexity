//
// Created by stephane on 19/07/18.
//

#include "Parser.h"
#include <boost/tokenizer.hpp>


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
        while(std::getline(filter_stream, line)) {
            if (i == 0)
                std::cout << line << std::endl;
            // split the line into fields
            int id;
            boost::tokenizer<boost::char_separator<char> > tok(line, sep);
            auto it= tok.begin();

            id = std::stoi(*it);

            if(i < 10)
                std::cout << id << std::endl;
            i++;
        }
        std::cout << i << " lines " << std::endl;

    }
}


ParserException::ParserException(std::string const& msg) throw() : m_msg(msg) {

}

