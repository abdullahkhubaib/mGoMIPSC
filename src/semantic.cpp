//
// Created by abdul on 2023-03-09.
//

#include "../include/semantic.h"


void semantic::check(Node* AST) {
    // Scans the global variables and function names.
    file_scope_check(AST);
    // Pass 2: Handles everything else.
    symbol_check(AST);
}

void semantic::file_scope_check(Node *AST) {
    stab.open_scope("file"); // Create the file table.
    // Iterate over the declarations in the file scope.
    for(Node* decl: AST->children) {
        // Function declaration
        if(decl->type == FUNC_DEF) {
            const int& lineNo = decl->children[0]->lineNo;
            // Search for the type and check if it is valid.
            Record* type = stab.lookup(decl->children[1]->children[1]->attr, lineNo); // Return type.
            if(!type->is_type)
                error("expected a type, but got '" + decl->children[1]->children[1]->attr +"' instead", lineNo);

            // Search the function body for return statements.
            if(type->sig != "void") {
                bool found = false;
                decl->children[2]->preorder([this, &found](Node* n, bool& running) {
                    if(n->type == RETURN_STMT) {
                        found = true;
                        running = false; // End the loop early.
                    }
                });
                if(!found)
                    error("no return statement in function", lineNo);
            }
            decl->children[1]->children[1]->sym = type; // Store the record address for the type.

            auto& formals = decl->children[1]->children[0]->children;

            // Create the function signature
            std::string sig = "f(";
            // Store the types of the parameters if the function has parameters.
            if(!formals.empty()) {
                // Reserve space for the string to reduce copying.
                sig.reserve(formals.size() * 4 + 9);
                for(Node* formal : formals)
                    sig += stab.lookup(formal->children[1]->attr, lineNo)->sig + ",";

                sig.pop_back(); // remove comma at the end
            }

            sig += ") " + type->sig;
            std::string& name = decl->children[0]->attr;
            decl->sym = stab.define(name, sig, lineNo);

            if(name == "main") {
                if(!formals.empty())
                    error("main() cannot have arguments.", lineNo);
                if(type->sig != "void")
                    error("main() cannot have a return type.", lineNo);
                AST->sym = decl->sym;
            }
        } else // Global variable declaration
            decl->sym = varDecl(decl);
    }
    if(AST->sym == nullptr)
        error("No main() function in program.");
}

void semantic::symbol_check(Node* AST) {
    // Iterate over the declarations in the file scope.
    for(Node* decl: AST->children) {
        if(decl->type == FUNC_DEF) {
            // Checks if the return type is still valid.
            Node* ret_type = decl->children[1]->children[1];
            if(!stab.lookup(ret_type->attr, ret_type->lineNo)->is_type)
                error("expected a type, but got '" + ret_type->attr +"' instead", ret_type->lineNo);
            // Create a new scope with the return type as the tag.
            stab.open_scope(ret_type->sym->sig);
            // Define the function parameters in the new scope.
            for(Node* formal : decl->children[1]->children[0]->children)
                formal->children[0]->sym = varDecl(formal);
            block_symbol_check(decl->children[2]);
        }
        // Checks if the global variable's type has been redefined.
        else if(Node* type = decl->children[1]; !stab.lookup(type->attr, type->lineNo)->is_type)
                error("expected a type, but got '" + type->attr +"' instead", type->lineNo);
    }
}

// Define a variable in the symbol table and return a pointer to it's record.
Record* semantic::varDecl(Node* decl) {
    const int& lineNo = decl->children[0]->lineNo;
    std::string& name = decl->children[0]->attr;
    // Search for the type and check if it is valid.
    Record* type = stab.lookup(decl->children[1]->attr, lineNo);
    if(!type->is_type)
        error("expected a type, but got '" + decl->children[1]->attr +"' instead", lineNo);

    decl->children[1]->sym = type; // Store the address of the type record.
    // Define the symbol into the table and return the address.
    return stab.define(name, type->sig, lineNo);
}

void semantic::block_symbol_check(Node* block) {
    for(Node* statement: block->children) {
        switch (statement->type) {
            case VAR_DEF:
                statement->sym = varDecl(statement);
                break;
            case ASSIGN_EXPR:
                if(Node* n = statement->children[0]; n->type == BOOL_T && stab.lookup(n->attr, n->lineNo)->sig != "bool")
                    n->type = ID_T;
                if(statement->children[0]->type == BOOL_T)
                    error("Can't assign to a constant.", statement->lineNo);
                else if(statement->children[0]->type != ID_T)
                    error("Can only assign to a variable.", statement->lineNo);
            case EXPR_STMT:
                expression_symbol_check(statement);
                break;
            case RETURN_STMT:
                // Type check the return value;
                if(stab.find_tag("void")) {
                    if(!statement->children.empty())
                        error("cannot return value in void function.", statement->lineNo);
                } else {
                    if(statement->children.empty())
                        error("must return something in non-void function.", statement->lineNo);
                    expression_symbol_check(statement->children[0]);
                    if(!stab.find_tag(statement->children[0]->sig))
                        error("wrong return type.", statement->lineNo);
                }
                break;
            case BREAK_STMT:
                if(!stab.find_tag("for"))
                    error("no for loop to break out of.", statement->lineNo);
                break;
            case IF_STMT:
            case IFELSE_STMT:
            case FOR_STMT:
                ifelse_for_symbol_check(statement);
                break;
            case BLOCK:
                stab.open_scope("block");
                block_symbol_check(statement);
                break;
            default:
                break;
        }
    }
    //std::cout << "\nBlock Scope starting @ line: " << block->children[0]->lineNo << "\n";
    //stab.print_scope();
    stab.close_scope();
}

// Every expression is traversed twice.
// First time determines types using preorder traversal.
// Second time checks them using postorder traversal.
void semantic::expression_symbol_check(Node* expression) {
    expression->postorder([this](Node* n, bool& running){
        switch(n->type) {
            case INT_T:
                // Integer must be a 32-bit signed integer.
                try { std::stoi(n->attr); }
                catch(std::exception& e) {
                    if(n->attr[0] == '-')
                        error("integer literal too small.", n->lineNo);
                    error("integer literal too large.", n->lineNo);
                }
                n->sig = "int";
                break;
            case STRING_T:
                n->sig = "str";
                break;
            case BOOL_T:
            case ID_T: {
                Record* id = stab.lookup(n->attr, n->lineNo);
                if(id->is_type)
                    error("'" + id->sig + "' type identifier is not allowed here.", n->lineNo);
                // convert bool to identifier if it has been redefined.
                if(n->type == BOOL_T && id->sig != "bool")
                    n->type = ID_T;
                std::string sig = id->sig;
                auto pos = sig.find(' ');
                // Only get the parameters if it's a function.
                n->sig = (pos == std::string::npos) ? sig : sig.substr(0, pos);
                n->sym = id;
                break;
            }
            case FUNCCALL:
                funccall_symbol_check(n);
                break;
            // Unary operators:
            case NOT_EXPR:
                if(n->children[0]->sig != "bool")
                    error("Type mismatch with operator '" + Node::typeNames[n->type] + "'", n->lineNo);
                n->sig = "bool";
                break;
            case UNARY_MINUS:
                if(n->children[0]->sig != "int")
                    error("Type mismatch with operator '" + Node::typeNames[n->type] + "'", n->lineNo);
                n->sig = "int";
                break;
            // Both sides must be bool.
            case OR_EXPR:
            case AND_EXPR:
                if(n->children[0]->sig != "bool" || n->children[1]->sig != "bool")
                    error("Type mismatch with operator '" + Node::typeNames[n->type] + "'", n->lineNo);
                n->sig = "bool";
                break;
            // For cases where both sides must have the same type.
            case ASSIGN_EXPR:
            case EQUAL_EXPR:
            case NOT_EQUAL_EXPR:
                if(n->children[0]->sig != n->children[1]->sig)
                    error("Type mismatch with operator '" + Node::typeNames[n->type] + "'", n->lineNo);
                // Sets it to void for assignment and bool for everything else.
                n->sig = (n->type == ASSIGN_EXPR) ? "void" : "bool";
                break;
            default:
                if(n->type >= 24) {
                    // Both operands must be int or string
                    if(n->children[0]->sig != n->children[1]->sig)
                        error("Type mismatch with operator '" + Node::typeNames[n->type] + "'", n->lineNo);
                    if(n->children[0]->sig != "int" && (n->type < 34 && n->children[0]->sig != "str"))
                        error("Type mismatch with operator '" + Node::typeNames[n->type] + "'", n->lineNo);

                    // int for arithmetic operators and bool for comparison operators.
                    n->sig = (n->type >= 34) ? "bool" : "int";
                }
        }
    });

}

// Checks if funccalls have the right type and number of parameters.
void semantic::funccall_symbol_check(Node *n) {
    if(n->children[0]->type != ID_T)
        error("function name must be an identifier.", n->children[0]->lineNo);
    Record* r = n->children[0]->sym;
    std::string sig = r->sig;
    auto pos = sig.find(' ');
    if(pos == std::string::npos)
        error("'" + n->children[0]->attr + "' is not a function.");
    n->sig = sig.substr(pos + 1);

    // Check if parameters match the required signature.
    // split the string by commas.
    sig = sig.substr(2, pos - 3);
    std::stringstream ss(sig);
    std::vector<std::string> actual_types;
    while(ss.good()) {
        std::string actual_type;
        getline(ss, actual_type, ',');
        if(!actual_type.empty())
            actual_types.push_back(actual_type);
    }
    auto& actuals = n->children[1]->children;
    if(actual_types.size() != actuals.size())
        error("expected " + std::to_string(actual_types.size()) + " parameters in function '" + n->children[0]->attr +
        "' but got " + std::to_string(actuals.size()), n->lineNo);

    for(int i = 0; i < (int) actual_types.size(); i++)
        if(actual_types[i] != actuals[i]->sig)
            error("type mismatch between parameters in function '" + n->children[0]->attr +  "'", n->lineNo);
}
// Checks for loops and if statements.
// Uses recursion to check if-else chains.
void semantic::ifelse_for_symbol_check(Node *n) {
    expression_symbol_check(n->children[0]);
    if(n->children[0]->sig != "bool") {
        if(n->type == FOR_STMT)
            error("for condition must be of type boolean", n->lineNo);
        error("if condition must be of type boolean", n->lineNo);
    }

    stab.open_scope(Node::typeNames[n->type]);
    block_symbol_check(n->children[1]);

    if(n->type == IF_STMT || n->type == FOR_STMT)
        return;

    // False if the else is followed by another if.
    if(n->children[2]->type == BLOCK) {
        stab.open_scope("else");
        block_symbol_check(n->children[2]);
    } else // Recursively check the if else chain.
        ifelse_for_symbol_check(n->children[2]);
}



