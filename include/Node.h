//
// Created by abdul on 2023-02-11.
//

#ifndef GOLF_NODE_H
#define GOLF_NODE_H
#include <vector>
#include <string>
#include <iostream>
#include <stack>
#include <utility>
#include <functional>
#include <algorithm>
#include <sstream>
#include <iomanip>

// 24 to 38 are operator expressions.
enum NODE_TYPE {PROGRAM, VAR_DEF, GLOBVAR_DEF, FUNC_DEF,  NEWID, TYPEID, SIG, FORMALS, FORMAL, BLOCK, INT_T, BOOL_T,
                STRING_T, ID_T, EMPTY_STMT, BREAK_STMT, EXPR_STMT, IF_STMT, IFELSE_STMT, RETURN_STMT, FOR_STMT,
                FUNCCALL, ACTUALS, UNARY_MINUS, ADD_EXPR, SUBTRACT_EXPR, MULTIPLY_EXPR, DIVIDE_EXPR, MOD_EXPR,
                AND_EXPR, OR_EXPR, NOT_EXPR, EQUAL_EXPR, NOT_EQUAL_EXPR, LESS_THAN_EXPR, GREATER_THAN_EXPR,
                ASSIGN_EXPR, LESS_EQUAL_EXPR, GREATER_EQUAL_EXPR};

class Node {
private:
    int m_depth;
public:
    const static std::string typeNames[];
    NODE_TYPE type;
    std::string attr;
    int lineNo;
    std::string sig;
    int sym;
    std::vector<Node*> children;
    Node(NODE_TYPE type, std::string attr, int lineNo);
    Node(NODE_TYPE type, int lineNo);
    Node(NODE_TYPE type, std::string attr);
    Node(NODE_TYPE type);
    ~Node();
    void addChild(Node* child);
    int max_depth();
    std::string to_string() const;
    void preorder(const std::function<void(Node*, bool&)>& func);
    void postorder(const std::function<void(Node*, bool&)>& func);
    void postorder_depth(const std::function<void(Node*, bool&)>& func);
    void display();
};


#endif //GOLF_NODE_H
