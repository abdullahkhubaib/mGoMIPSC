//
// Created by abdul on 2023-03-09.
//

#include "../include/SymbolTable.h"

static int num_records = 0;

Record::Record(const std::string& sig, bool is_t): sig(sig), is_type(is_t) {
    id = num_records++;
}

SymbolTable::SymbolTable() {
    //ScopeStack.reserve(8); // Reserve memory for 8 levels of the stack.
    ScopeStack.emplace_back(Table{
            {"$void", Record("void", true)},
            {"bool", Record("bool", true)},
            {"int", Record("int", true)},
            {"string", Record("str", true)},
            {"$true", Record("bool", false)},
            {"true", Record("bool", false)},
            {"false", Record("bool", false)},
            {"printb", Record("f(bool) void", false)},
            {"printc", Record("f(int) void", false)},
            {"printi", Record("f(int) void", false)},
            {"prints", Record("f(str) void", false)},
            {"getchar", Record("f() int", false)},
            {"halt", Record("f() void", false)},
            {"len", Record("f(str) int", false)}
    }, "universal");
}


// Tag stores information about the creator of the scope.
void SymbolTable::open_scope(const std::string& tag) {
    ScopeStack.emplace_back(Table(), tag);
}

void SymbolTable::print_scope() {
    // Print all the records in the table.
    for(auto& [name, record]: ScopeStack.back().first)
        std::cout << name << ": " << record.sig << "\n";
}

void SymbolTable::close_scope(){
    ScopeStack.pop_back();
}

Record& SymbolTable::define(const std::string &name, const std::string &sig, int lineNo) {
    if(ScopeStack.back().first.count(name))
        error("'" + name + "' is being redefined.", lineNo);
    // Defines the new record and returns its address.
    return ScopeStack.back().first.emplace(name, Record(sig, false)).first->second;
}

Record& SymbolTable::lookup(const std::string& name, int lineNo) {
    // Iterate through the stack in reverse due to it being a vector.
    for(auto it = ScopeStack.rbegin(); it != ScopeStack.rend(); it++)
        // Search for the key and return it if found.
        if(const auto& key = it->first.find(name); key != it->first.end())
            return key->second;

    error("'" + name + "' is not defined.", lineNo);
    exit(1);
}

bool SymbolTable::find_tag(const std::string &tag) {
    for(auto it = ScopeStack.rbegin(); it != ScopeStack.rend(); it++)
        if(tag == it->second)
            return true;
    return false;
}






