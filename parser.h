//
// Created by abdul on 2023-02-12.
//

#ifndef GOLF_PARSER_H
#define GOLF_PARSER_H
#include "scanner.h"
#include "Node.h"
#include "Token.h"
#include "error.h"
#include <vector>
#include <algorithm>

class Parser {
public:
    Parser(Scanner* s);
    Node* parse();
private:
    Scanner* scanner;
    Node* AST;
    Token t;
    Token expect(TYPE type);
    Token expect(TYPE type, const std::string& message);
    static void semicolon_error(const Token& tok);
    Node* var(bool global);
    Node* func();
    Node* block();
    Node* expression(int k);
    static int precedence(TYPE t);
    Node* factor();
    Node* ifStmt();
    Node* returnStmt();
    Node* forStmt();
};


#endif //GOLF_PARSER_H
