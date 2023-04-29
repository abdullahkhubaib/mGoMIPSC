//
// Created by abdul on 2023-01-28.
//

#ifndef GOLF_SCANNER_H
#define GOLF_SCANNER_H

#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include "Token.h"
#include "error.h"


class Scanner {
private:
    std::ifstream file;
    int lineNo;
    bool reread;
    bool rereadT;
    char last;
    Token lastT;
    char read();
    void unread();
    const static std::unordered_map<std::string, TYPE> keywords;
public:

    Scanner(const std::string& fileName);
    Token lex();
    void unlex();

};


#endif //GOLF_SCANNER_H
