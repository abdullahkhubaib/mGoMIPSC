//
// Class for modeling the tokens
//

#ifndef GOLF_TOKEN_H
#define GOLF_TOKEN_H

#include <string>
#include <utility>

// 1 to 8 have an auto semicolon.
enum TYPE {EOFILE, ID, BREAK, INT, STRING, RETURN, CLOSE_BRACKET, CLOSE_BRACE, BOOL,
              ELSE, FOR, FUNC, IF, VAR, ADD, SUBTRACT, MULTIPLY, DIVIDE, MOD, AND,
              OR, NOT, EQUAL, NOT_EQUAL, LESS_THAN, GREATER_THAN, ASSIGN, LESS_EQUAL,
              GREATER_EQUAL, OPEN_BRACKET, OPEN_BRACE, COMMA, SEMICOLON};


class Token {
public:
    const static std::string typeNames[];
    TYPE type;
    std::string attr;
    int where;
    Token() = default;
    Token(TYPE type, std::string attr, int where);
    Token(TYPE type, char attr, int where);
    Token(TYPE type, int where);
    Token& operator= (const Token & rhs);
    std::string to_string() const;
    std::string display() const;
};


#endif //GOLF_TOKEN_H
