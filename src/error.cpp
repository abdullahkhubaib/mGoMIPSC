//
// Created by abdul on 2023-01-28.
//

#include "../include/error.h"


int warnings = 0;

void error(const std::string& message, int lineNo) {
    std::cerr << "Error at line " << lineNo << ": " << message << "\n";
    exit(1);
}

void error(const std::string& message) {
    std::cerr << "Error: " << message << "\n";
    exit(1);
}

void warning(const std::string& message, int lineNo) {
    std::cerr << "Warning at line " << lineNo << ": " << message << "\n";
    warnings++;
    if(warnings > MAX_WARNINGS)
        error("too many warnings.", lineNo);
}


