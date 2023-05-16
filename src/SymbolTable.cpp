//
// Created by abdul on 2023-03-09.
//

#include "../include/SymbolTable.h"


SymbolTable::SymbolTable() {
    //ScopeStack.reserve(8); // Reserve memory for 8 levels of the stack.
    open_scope("universal");
    Table& t = ScopeStack.back().first;
    t["$void"]   = std::make_unique<Record>("void", true);
    t["bool"]    = std::make_unique<Record>("bool", true);
    t["int"]     = std::make_unique<Record>("int", true);
    t["string"]  = std::make_unique<Record>("str", true);
    t["$true"]   = std::make_unique<Record>("bool", false);
    t["true"]    = std::make_unique<Record>("bool", false);
    t["false"]   = std::make_unique<Record>("bool", false);
    t["printb"]  = std::make_unique<Record>("f(bool) void", false);
    t["printc"]  = std::make_unique<Record>("f(int) void", false);
    t["printi"]  = std::make_unique<Record>("f(int) void", false);
    t["prints"]  = std::make_unique<Record>("f(str) void", false);
    t["getchar"] = std::make_unique<Record>("f() int", false);
    t["halt"]    = std::make_unique<Record>("f() void", false);
    t["len"]     = std::make_unique<Record>("f(str) int", false);
}

// Tag stores information about the creator of the scope.
void SymbolTable::open_scope(const std::string& tag) {
    ScopeStack.emplace_back(Table(), tag);
}

void SymbolTable::print_scope() {
    // Print all the records in the table.
    for(auto& [name, record]: ScopeStack.back().first)
        std::cout << name << ": " << record->sig << "\n";
}

void SymbolTable::close_scope(){
    ScopeStack.pop_back();
}

Record* SymbolTable::define(const std::string &name, const std::string &sig, int lineNo) {
    if(ScopeStack.back().first.count(name))
        error("'" + name + "' is being redefined.", lineNo);
    // Defines the new record and returns its address.
    return ScopeStack.back().first.emplace(name, std::make_unique<Record>(sig, false)).first->second.get();
}

Record* SymbolTable::lookup(const std::string& name, int lineNo) {
    // Iterate through the stack in reverse due to it being a vector.
    for(auto it = ScopeStack.rbegin(); it != ScopeStack.rend(); it++)
        // Search for the key and return it if found.
        if(const auto& key = it->first.find(name); key != it->first.end())
            return key->second.get();

    error("'" + name + "' is not defined.", lineNo);
    return nullptr;
}

bool SymbolTable::find_tag(const std::string &tag) {
    for(auto it = ScopeStack.rbegin(); it != ScopeStack.rend(); it++)
        if(tag == it->second)
            return true;
    return false;
}




