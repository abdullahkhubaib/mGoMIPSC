//
// Created by abdul on 2023-03-27.
//

#ifndef GOLF_GEN_H
#define GOLF_GEN_H

#include "Node.h"
#include "error.h"
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <regex>
#include <set>
#include <unordered_set>

class gen {
private:
    static std::string emit_label(const std::string& label);
    static std::string emit(const std::string& str);
    Node* AST;
    std::unordered_map<std::string, std::string> strings; // store string literals and their corresponding labels.
    const std::string& stoLabel(std::string& string); // convert string to its label.
    std::unordered_map<Record*, std::string> glob_vars; // store global variables and their corresponding labels.
    std::unordered_map<Record*, std::string> local_vars;
    std::unordered_map<Record*, std::string> funcs;
    std::unordered_set<std::string> predef_used;
    static const std::unordered_map<std::string, std::string> predef;
    void gen_func(Node *func, std::stringstream &ss);
    void gen_block(Node *block, std::stringstream &ss, Node *func, int &lNum);
    void gen_expression(Node *expression, std::stringstream &ss);
    void gen_short_circuit(Node *n, std::stringstream &ss, Node *func, int &lNum);
    void gen_if_else(Node *statement, std::stringstream &ss, Node *func, int &lNum, std::string endL);
    std::stack<std::string> loop_end;
    void gen_for(Node *statement, std::stringstream &ss, Node *func, int &lNum);
    std::vector<bool> registers;
    std::string get_register();
    void free_register(const std::string& r);
public:
    gen(Node* AST);
    std::string generate();
};


#endif //GOLF_GEN_H
