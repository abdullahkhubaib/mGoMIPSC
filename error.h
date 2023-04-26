//
// Created by abdul on 2023-01-28.
//

#ifndef GOLF_ERROR_H
#define GOLF_ERROR_H
#define MAX_WARNINGS 10

#include <iostream>

extern int warnings;

void warning(const std::string& message, int lineNo);

void error(const std::string& message, int lineNo);
void error(const std::string& message);

#endif //GOLF_ERROR_H
