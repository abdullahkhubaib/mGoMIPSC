//
// Created by abdul on 2023-02-12.
//


#include "parser.h"

Parser::Parser(Scanner *s): scanner(s), AST(nullptr) {
}

Node* Parser::parse() {
    if(AST != nullptr)
        return AST;
    AST = new Node(PROGRAM);
    while(true)
    {
        t = scanner->lex();
        Node* n = nullptr;
        switch (t.type) {
            case EOFILE:
                return AST;
            case FUNC:
                n = func();
                break;
            case VAR:
                n = var(true);
                break;
            default:
                error("Expected a declaration but got: " + t.to_string(), t.where);
        }
        AST->addChild(n);
    }
}

Token Parser::expect(TYPE type) {
    Token tok = scanner->lex();
    if(tok.type != type)
    {
        semicolon_error(tok);
        error("Expected: '" + Token::typeNames[type] + "' but got: " + tok.to_string(), tok.where);
    }
    return tok;
}

Token Parser::expect(TYPE type, const std::string& message) {
    Token tok = scanner->lex();
    if(tok.type != type)
        error(message, tok.where);
    return tok;
}

// Checks for illegal semicolons
void Parser::semicolon_error(const Token& tok) {
    if(tok.type == SEMICOLON)
    {
        if(tok.attr.empty())
            error("Unexpected newline", tok.where);
        else
            error("Unexpected ;", tok.where);
    }
}

// Handles both local and global variables
Node* Parser::var(bool global) {
    Node* head = new Node(static_cast<NODE_TYPE>(global + 1), t.where);
    t = scanner->lex();
    if(t.type != ID && t.type != BOOL)
    {
        semicolon_error(t);
        error("Expected: identifier but got: " + t.to_string(), t.where);
    }
    head->addChild(new Node(NEWID, t.attr, t.where));
    t = expect(ID);
    head->addChild(new Node(TYPEID, t.attr, t.where));
    expect(SEMICOLON);
    return head;
}

Node* Parser::func() {
    Node* head = new Node(FUNC_DEF, t.where);
    t = expect(ID, "Expected a name for the function.");
    // The name of the function.
    head->addChild(new Node(NEWID, t.attr, t.where));
    expect(OPEN_BRACKET);
    Node* sig = new Node(SIG);

    Node* formals = new Node(FORMALS);
    t = scanner->lex();
    while(t.type != CLOSE_BRACKET)
    {
        if(t.type != ID)
            error("parameter name must be an identifier but got: " + t.to_string(), t.where);
        Node* formal = new Node(FORMAL);
        // Name of the parameter
        formal->addChild(new Node(NEWID, t.attr, t.where));
        t = expect(ID, "type name must be an identifier but got: " + t.to_string());
        // parameter type
        formal->addChild(new Node(TYPEID, t.attr, t.where));
        formals->addChild(formal);
        t = scanner->lex();

        semicolon_error(t);
        if(t.type != COMMA && t.type != CLOSE_BRACKET)
            error("Expected: ', or )' but got: " + t.to_string(), t.where);

        if(t.type == COMMA)
            t = scanner->lex();
    }

    sig->addChild(formals);

    t = scanner->lex();
    // Return type if given
    if(t.type == ID)
    {
        sig->addChild(new Node(TYPEID, t.attr, t.where));
        t = scanner->lex();
    } else
        sig->addChild(new Node(TYPEID, "$void"));

    head->addChild(sig);

    semicolon_error(t);
    if(t.type != OPEN_BRACE)
        error("Expected: '{' but got: " + t.to_string(), t.where);

    head->addChild(block());


    return head;
}

Node* Parser::block() {
    if(t.type != OPEN_BRACE)
        error("Expected: '{' but got: " + t.to_string(),t.where);
    Node* head = new Node(BLOCK);
    t = scanner->lex();
    if(t.type == CLOSE_BRACE)
        head->addChild(new Node(EMPTY_STMT));
    while(t.type != CLOSE_BRACE)
    {
        // std::cout << t.to_string() << "\n";
        switch (t.type) {
            case VAR:
                head->addChild(var(false));
                break;
            case RETURN:
                head->addChild(returnStmt());
                break;
            case BREAK:
                head->addChild(new Node(BREAK_STMT, t.where));
                expect(SEMICOLON);
                break;
            case OPEN_BRACE:
                head->addChild(block());
                break;
            case IF:
                head->addChild(ifStmt());
                break;
            case FOR:
                head->addChild(forStmt());
                break;
            case EOFILE:
                error("Block never closed.", t.where);
                break;
            case SEMICOLON:
                head->addChild(new Node(EMPTY_STMT));
                break;
            case ID:
            case INT:
            case BOOL:
            case SUBTRACT: // Starts with unary minus
            case NOT: // starts with unary not.
            case OPEN_BRACKET:
            case STRING:
                head->addChild(expression(0));
                if(head->children.back()->type != ASSIGN_EXPR)
                {
                    Node* n = head->children.back();
                    head->children.back() = new Node(EXPR_STMT, n->lineNo);
                    head->children.back()->addChild(n);
                }
                break;
            default:
                error("Invalid Token: " + t.to_string(), t.where);
        }
        t = scanner->lex();
    }
    Token t2 = scanner->lex();
    if(t2.type != SEMICOLON)
        scanner->unlex();
    else  t = t2;
    return head;
}

// Returns the precedence level for a given operator and -1 if not an operator.
int Parser::precedence(TYPE t) {
    switch(t)
    {
        case ASSIGN:
            return 0;
        case OR:
            return 1;
        case AND:
            return 2;
        case EQUAL:
        case NOT_EQUAL:
        case LESS_THAN:
        case LESS_EQUAL:
        case GREATER_THAN:
        case GREATER_EQUAL:
            return 3;
        case ADD:
        case SUBTRACT:
            return 4;
        case MULTIPLY:
        case DIVIDE:
        case MOD:
            return 5;
        default:
            return -1;
    }
}

// An implementation of a pratt parser.
// k is the minimum precedence level that is allowed to be traversed.
// Only calling with 0 allows assignment.
Node* Parser::expression(int k) {
    Node* left = factor();

    // If the operator is greater than the current level then it will be parsed.
    // Otherwise, it drops down a precedence level.
    while(precedence(t.type) >= k)
    {
        Token op = t;
        t = scanner->lex();

        // Only allow one assignment in a statement.
        if(op.type == ASSIGN && left->type == ASSIGN_EXPR)
            error("Illegal = in expression", op.where);

        // Create an expression at one level lower than the current one.
        Node* head = new Node(static_cast<NODE_TYPE>(op.type + 10), op.where);
        head->addChild(left);
        head->addChild(expression(precedence(op.type) + 1));
        left = head;
    }

    return left;

}

Node* Parser::factor() {
    Node* head = nullptr;

    switch (t.type) {
        case OPEN_BRACKET:
            t = scanner->lex();
            head = expression(1);
            //Node::display(head);
            if(t.type != CLOSE_BRACKET)
                error("Expected: ')' but got: " + t.to_string(), t.where);
            t = scanner->lex();
            break;
        case SUBTRACT:
            head = new Node(UNARY_MINUS, t.where);
            t = scanner->lex();
            head->addChild(factor());

            if(head->children[0]->type == UNARY_MINUS)
                head = head->children[0];

            if(head->children[0]->type == INT_T && head->children[0]->attr[0] != '-')
            {
                head = head->children[0];
                head->attr = "-" + head->attr;
            }
            break;
        case NOT:
            head = new Node(NOT_EXPR, t.where);
            t = scanner->lex();
            head->addChild(factor());
            switch(head->children[0]->type) {
                case NOT_EXPR:
                    head = head->children[0];
                    break;
                case EQUAL_EXPR:
                    head = head->children[0];
                    head->type = NOT_EQUAL_EXPR;
                    break;
                case NOT_EQUAL_EXPR:
                    head = head->children[0];
                    head->type = EQUAL_EXPR;
                    break;
                case LESS_THAN_EXPR:
                    head = head->children[0];
                    head->type = GREATER_EQUAL_EXPR;
                    break;
                case GREATER_THAN_EXPR:
                    head = head->children[0];
                    head->type = LESS_EQUAL_EXPR;
                    break;
                case LESS_EQUAL_EXPR:
                    head = head->children[0];
                    head->type = GREATER_THAN_EXPR;
                    break;
                case GREATER_EQUAL_EXPR:
                    head = head->children[0];
                    head->type = LESS_THAN_EXPR;
                    break;
                default:
                    break;
            }
            break;
        case ID:
            head = new Node(ID_T, t.attr, t.where);
            t = scanner->lex();
            break;
        case INT:
            head = new Node(INT_T, t.attr, t.where);
            t = scanner->lex();
            break;
        case BOOL:
            head = new Node(BOOL_T, t.attr, t.where);
            t = scanner->lex();
            break;
        case STRING:
            head = new Node(STRING_T, t.attr, t.where);
            t = scanner->lex();
            break;
        default:
            error("Unrecognized Token: " + t.to_string());
    }

    // Handle function calls.
    if(t.type == OPEN_BRACKET)
    {
        Node* n = head;
        head = new Node(FUNCCALL, t.where);
        head->addChild(n);
        n = new Node(ACTUALS);
        head->addChild(n);
        t = scanner->lex();
        while(t.type != CLOSE_BRACKET)
        {
            n->addChild(expression(1));
            //std::cout << t.to_string() << "\n";
            semicolon_error(t);
            if(t.type != COMMA && t.type != CLOSE_BRACKET)
                error("Expected: ', or )' but got: " + t.to_string(), t.where);

            if(t.type != CLOSE_BRACKET)
                t = scanner->lex();
            if(t.type == COMMA)
                t = scanner->lex();
        }
        t = scanner->lex();

    }

    return head;
}

Node* Parser::ifStmt() {
    Node* head = new Node(IF_STMT, t.where);
    t = scanner->lex();
    // Parse the condition
    head->addChild(expression(1));
    semicolon_error(t);

    head->addChild(block());
    if(t.type == SEMICOLON)
        return head;
    t = scanner->lex();
    if(t.type != ELSE)
        error("Invalid " + t.to_string(), t.where);
    head->type = IFELSE_STMT;
    t = scanner->lex();

    if(t.type == IF)
        head->addChild(ifStmt());
    else if(t.type == OPEN_BRACE)
        head->addChild(block());
    else
        error("Expected: '{' but got: " + t.to_string(), t.where);
    return head;
}

Node* Parser::returnStmt() {
    Node* head = new Node(RETURN_STMT, t.where);
    t = scanner->lex();
    if(t.type == SEMICOLON)
        return head;
    head->addChild(expression(1));
    if(t.type != SEMICOLON)
        scanner->unlex();
    return head;
}

Node* Parser::forStmt() {
    Node* head = new Node(FOR_STMT, t.where);
    t = scanner->lex();

    if(t.type == SEMICOLON)
        error("Unexpected ;", t.where);
    if(t.type == OPEN_BRACE)
        head->addChild(new Node(ID_T, "$true"));
    else
        head->addChild(expression(1));
    semicolon_error(t);
    head->addChild(block());
    return head;
}







