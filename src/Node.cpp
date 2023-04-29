//
// Created by abdul on 2023-02-11.
//


#include "../include/Node.h"

const std::string Node::typeNames[] = {
        "program", "var", "globvar", "func", "newid", "typeid", "sig", "formals",
        "formal", "block", "int", "bool", "string", "id", "emptystmt", "break",
        "exprstmt", "if", "ifelse", "return", "for", "funccall", "actuals", "u-",
        "+", "-", "*", "/", "%", "&&", "||", "!", "==", "!=", "<",
        ">", "=", "<=", ">="};

Node::Node(NODE_TYPE type, std::string attr, int lineNo)
    : type(type), attr(std::move(attr)), lineNo(lineNo)  {}

Node::Node(NODE_TYPE type, int lineNo)
    : type(type), lineNo(lineNo)  {}

Node::Node(NODE_TYPE type, std::string attr)
    : type(type), attr(std::move(attr)), lineNo(-1) {}

Node::Node(NODE_TYPE type)
    : type(type), lineNo(-1)  {}

// Recursively delete all children in the tree.
Node::~Node() {
    for(Node* n: children)
        delete n;
}

void Node::addChild(Node* child) {
    children.emplace_back(child);
}

std::string Node::to_string() const {
    std::stringstream ss;
    ss << typeNames[type];
    if(!attr.empty())
        ss << " [" << attr << "]";
    if(!sig.empty())
        ss << " sig=" << sig;
    if(sym != nullptr) {
        // displays the pointer as a hex number with 16 digits and underscores separating the bytes.
        std::stringstream addrS;
        // Convert to hex and pad with zeroes to create a 16 character long string
        addrS << std::hex << std::setw(16) << std::setfill('0') << reinterpret_cast<std::uintptr_t>(sym);
        std::string addr = addrS.str();
        std::string formatted;
        formatted.reserve(32);
        // Add underscores between every 2 characters.
        for (int i = 0; i < 16; i += 2)
            formatted += addr.substr(i, 2) + "_";
        // Remove last underscore.
        formatted.pop_back();
        ss << " sym=0x" << formatted;
    }
    if(lineNo > -1)
        ss << " @ line " << lineNo;
    return ss.str();
}

// Uses a stack to perform preorder traversal.
void Node::preorder(const std::function<void(Node*, bool&)> &func) {
    std::stack<Node*> stack;
    stack.emplace(this);

    while(!stack.empty()) {
        Node* n = stack.top();
        stack.pop();
        // Setting running to false in provided function ends the traversal early
        bool running = true;
        func(n, running);
        if(!running) return;
        // Iterate through the children in reverse and add to the stack.
        for (auto it = n->children.rbegin(); it != n->children.rend(); it++)
            stack.emplace(*it);
    }
}


// Uses a stack to perform postorder traversal.
void Node::postorder(const std::function<void(Node *, bool&)> &func) {
    std::stack<std::pair<Node*, bool>> stack; // Second value is true if the node has been visited.
    stack.emplace(this, false);
    while(!stack.empty()) {
        auto& [n, visited] = stack.top();

        // if the current node has no children or if they've all been visited.
        if (n->children.empty() || visited) {
            stack.pop();

            // Setting running to false in provided function ends the traversal early
            bool running = true;
            func(n, running);
            if(!running) return;
        }
        // Iterate through the children in reverse and add to the stack.
        else {
            visited = true;
            for (auto it = n->children.rbegin(); it != n->children.rend(); it++)
                stack.emplace(*it, 0);
        }

    }
}

// Uses a Stack to display the tree
void Node::display() {
    std::stack<std::pair<Node*, int>> stack;
    stack.emplace(this, 0);

    while (!stack.empty()) {
        auto [n, depth] = stack.top();
        stack.pop();

        for (int i = 0; i < depth; i++)
            std::cout << "    ";

        std::cout << n->to_string() << "\n";
        // Iterate through the children in reverse and add to the stack.
        for (auto it = n->children.rbegin(); it != n->children.rend(); it++)
            stack.emplace(*it, depth + 1);
    }
}

// Uses recursion to find the max depth of the tree.
int Node::max_depth() {
    if(m_depth != 0)
        return m_depth;
    if(children.empty()) return 1;
    std::vector<int> depths(children.size());
    for(int i = 0; i < (int) children.size(); i++)
        depths[i] = children[i]->max_depth();
    m_depth = 1 + *max_element(depths.begin(), depths.end());
    return m_depth;
}

