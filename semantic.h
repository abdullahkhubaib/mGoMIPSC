//
// Created by abdul on 2023-03-09.
//

#ifndef GOLF_SEMANTIC_H
#define GOLF_SEMANTIC_H


#include "Node.h"
#include "SymbolTable.h"
#include "string"
#include "sstream"
#include "scanner.h"


class semantic {
private:
    SymbolTable stab;
    void file_scope_check(Node* AST); // pass 1
    void symbol_check(Node* AST); // pass 2
    void block_symbol_check(Node* block);
    Record* varDecl(Node* decl);
    void expression_symbol_check(Node* expression);
    void funccall_symbol_check(Node* n);
    void ifelse_for_symbol_check(Node* n);
public:
    void check(Node* AST);

};




#endif //GOLF_SEMANTIC_H
