//
// Created by abdul on 2023-01-28.
//

#include "../include/Token.h"



const std::string Token::typeNames[] =
        {"EOF", "id", "break", "int", "string", "return", ")", "}",
         "bool", "else", "for", "func", "if",  "var", "+", "-",
         "*", "/", "%", "&&", "||", "!", "==", "!=", "<",
         ">", "=", "<=", ">=", "(", "{", ",", ";"};



Token::Token(TYPE type, std::string attr, int where)
    : type(type), attr(std::move(attr)), where(where) {}

Token::Token(TYPE type, char attr, int where)
        : type(type), attr(std::string(1, attr)), where(where) {}

// For tokens where the attribute is the same as the type.
Token::Token(TYPE type, int where)
        : type(type), attr(typeNames[type]), where(where) {}

// Creates a formatted string to display for the token.
std::string Token::to_string() const {
    return typeNames[type] + " [" + attr + "] @ line " + std::to_string(where);
}

// Creates a formatted string to display for the token with indentation.
std::string Token::display() const {
    // Displays
    std::string tName = typeNames[type];
    // Pad string to a length of 8 with whitespace.
    tName.insert(tName.size(), 8 - tName.size(), ' ');
    return tName + "[" + attr + "] @ line " + std::to_string(where);
}

Token &Token::operator=(const Token &rhs) {
    if(this == &rhs)
        return *this;
    type = rhs.type;
    attr = rhs.attr;
    where = rhs.where;
    return *this;
}




