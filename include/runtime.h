//
// Created by abdul on 2023-05-15.
//

#ifndef GOLF_RUNTIME_H
#define GOLF_RUNTIME_H
#include "error.h"
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <sstream>

class runtime {
private:
    static const std::unordered_map<std::string, std::string> funcs;
    std::unordered_set<std::string> used;
public:
    static const std::string header;
    void include(const std::string& name);
    std::string get();
};


#endif //GOLF_RUNTIME_H
