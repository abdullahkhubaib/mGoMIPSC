//
// Created by abdul on 2023-03-09.
//

#include "SymbolTable.h"


SymbolTable::SymbolTable() {
    //ScopeStack.reserve(8); // Reserve memory for 8 levels of the stack.
    ScopeStack.emplace_back(Table{
        {"$void", new Record{"void", true}},
        {"bool", new Record{"bool", true}},
        {"int", new Record{"int", true}},
        {"string", new Record{"str", true}},
        {"$true", new Record{"bool", false}},
        {"true", new Record{"bool", false}},
        {"false", new Record{"bool", false}},
        {"printb", new Record{"f(bool) void", false}},
        {"printc", new Record{"f(int) void", false}},
        {"printi", new Record{"f(int) void", false}},
        {"prints", new Record{"f(str) void", false}},
        {"getchar", new Record{"f() int", false}},
        {"halt", new Record{"f() void", false}},
        {"len", new Record{"f(str) int", false}}
    }, "universal");
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
    // Delete all the records in the table before deleting it.
    for(auto& [name, record]: ScopeStack.back().first)
        delete record;
    ScopeStack.pop_back();
}

Record* SymbolTable::define(const std::string &name, const std::string &sig, int lineNo) {
    if(ScopeStack.back().first.count(name))
        error("'" + name + "' is being redefined.", lineNo);
    // Defines the new record and returns its address.
    return ScopeStack.back().first.emplace(name, new Record{sig, false}).first->second;
}

Record* SymbolTable::lookup(const std::string& name, int lineNo) {
    // Iterate through the stack in reverse due to it being a vector.
    for(auto it = ScopeStack.rbegin(); it != ScopeStack.rend(); it++)
        // Search for the key and return it if found.
        if(const auto& key = it->first.find(name); key != it->first.end())
            return key->second;

    error("'" + name + "' is not defined.", lineNo);
    return nullptr;
}

bool SymbolTable::find_tag(const std::string &tag) {
    for(auto it = ScopeStack.rbegin(); it != ScopeStack.rend(); it++)
        if(tag == it->second)
            return true;
    return false;
}




