//
// Created by stephane on 19/07/18.
//
#include <fstream>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <QtCore/QString>
#include <exception>
#include <filesystem>
#include <iostream>
#include <string>

namespace fs = std::filesystem;


#ifndef SPDCOMPLEXITY_PARSER_H
#define SPDCOMPLEXITY_PARSER_H


class Parser {
public:
    Parser(int ctx);
    void parse_directory(QString &dir);
private:
    void parse_file(const fs::path &path);

};

class ParserException : public std::exception {
public:
    ParserException(std::string const& msg ="") throw();
private:
    int m_ctx;
    std::string m_msg;
};


#endif //SPDCOMPLEXITY_PARSER_H
