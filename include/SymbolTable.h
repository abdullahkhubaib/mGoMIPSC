//
// Created by abdul on 2023-03-09.
//

#ifndef GOLF_SYMBOLTABLE_H
#define GOLF_SYMBOLTABLE_H
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include "error.h"

struct Record {
    std::string sig;
    bool is_type;
    Record(const std::string& s, bool is_t) : sig(s), is_type(is_t) {};
};

using Table = std::unordered_map<std::string, std::unique_ptr<Record>>;

class SymbolTable {

private:
    // The second argument contains information about the scope creator.
    std::vector<std::pair<Table, std::string>> ScopeStack;

public:
    SymbolTable();
    void open_scope(const std::string& tag);
    void print_scope();
    void close_scope();
    Record* define(const std::string& name, const std::string& sig, int lineNo);
    Record* lookup(const std::string& name, int lineNo);
    bool find_tag(const std::string& tag);
};


#endif //GOLF_SYMBOLTABLE_H
