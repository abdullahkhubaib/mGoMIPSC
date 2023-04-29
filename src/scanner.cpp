//
// Created by abdul on 2023-01-28.
//

#include "scanner.h"

Scanner::Scanner(const std::string& fileName)
        : file(fileName), lineNo(1), reread(false), rereadT(false), last(0), lastT(EOFILE, -1)  {
    if(!file)
        error("File: " + fileName + " does not exist");
}

const std::unordered_map<std::string, TYPE> Scanner::keywords = {
        {"break", BREAK},
        {"else", ELSE},
        {"for", FOR},
        {"func", FUNC},
        {"if", IF},
        {"return", RETURN},
        {"var", VAR},
        {"true", BOOL},
        {"false", BOOL}
};

void Scanner::unread() {
    reread = true;
}

char Scanner::read() {
    if(reread)
    {
        reread = false;
        return last;
    }
    last = file.get();
    return last;
}

void Scanner::unlex() {
    rereadT = true;
}

Token Scanner::lex() {
    if(rereadT)
    {
        rereadT = false;
        return lastT;
    }

    // Stores literals, identifier names and keywords.
    std::string literal;

    START:
    char c = read();
    // Skip Whitespace
    while(isspace(c))
    {
        if(c == '\n')
        {
            // Inserts a semicolon at the end of the line if one is needed.
            // 1 to 8 have an auto semicolon.
            if(lastT.type > 0 && lastT.type <= 8)
            {
                lastT = Token(SEMICOLON, "", lineNo++);
                return lastT;
            }
            lineNo++;
        }
        else if(c == '\v')
            warning("Skipping unknown input character: '\\v'", lineNo);
        else if(c == '\f')
            warning("Skipping unknown input character: '\\f'", lineNo);
        c = read();
    }

    switch(c)
    {
        case -1: goto S1;  // End of File.
        case '+': goto S2;
        case '-': goto S3;
        case '*': goto S4;
        case '/': goto S5;
        case '%': goto S6;
        case '&': goto S7;
        case '|': goto S8;
        case '=': goto S9;  // Assignment and equals.
        case '<': goto S10; // Less than and less than or equal
        case '>': goto S11; // Greater than and greater than or equal
        case '!': goto S12; // Not and not equals.
        case '(': goto S13;
        case '{': goto S14;
        case ',': goto S15;
        case ')': goto S16;
        case '}': goto S17;
        case ';': goto S18;
        case '"': goto S19; // Handles string literals.
        default:
            if(std::isdigit(c)) goto S20; // Handles integer literals.
            else if(c == '_' || std::isalpha(c)) goto S21; // Keywords and identifiers.
            else goto S22;
    }

    S1:
    // Inserts a semicolon at the end of the file if one is needed.
    // 1 to 8 have an auto semicolon.
    if(lastT.type > 0 && lastT.type <= 8)
    {
        lastT = Token(SEMICOLON, "", lineNo);
        return lastT;
    }

    lastT = Token(EOFILE, lineNo);
    return lastT;

    S2:
    lastT = Token(ADD, lineNo);
    return lastT;

    S3:
    lastT = Token(SUBTRACT, lineNo);
    return lastT;
    S4:
    lastT = Token(MULTIPLY, lineNo);
    return lastT;

    S5:
    // Skips until the next line if it's a comment.
    if(read() == '/') {
        char ch = read();
        while(ch != '\n' && ch != -1) ch = read();
        unread();
        goto START;
    }
    unread();
    lastT = Token(DIVIDE, lineNo);
    return lastT;

    S6:
    lastT = Token(MOD, lineNo);
    return lastT;

    S7:
    if(read() != '&')
        error("Bitwise AND operation is not supported.", lineNo);
    lastT = Token(AND, lineNo);
    return lastT;

    S8:
    if(read() != '|')
        error("Bitwise OR operation is not supported.", lineNo);
    lastT = Token(OR, lineNo);
    return lastT;

    S9:
    if(read() == '=') // For equals
    {
        lastT = Token(EQUAL, lineNo);
        return lastT;
    }
    unread();
    lastT = Token(ASSIGN, lineNo);
    return lastT;

    S10:
    if(read() == '=') // For less than or equal
    {
        lastT = Token(LESS_EQUAL, lineNo);
        return lastT;
    }
    unread();
    lastT = Token(LESS_THAN, lineNo);
    return lastT;

    S11:
    if(read() == '=') // For greater than or equal
    {
        lastT = Token(GREATER_EQUAL, lineNo);
        return lastT;
    }
    unread();
    lastT = Token(GREATER_THAN, lineNo);
    return lastT;

    S12:
    if(read() == '=') // For not equals
    {
        lastT = Token(NOT_EQUAL, lineNo);
        return lastT;
    }
    unread();
    lastT = Token(NOT, lineNo);
    return lastT;

    S13:
    lastT = Token(OPEN_BRACKET, lineNo);
    return lastT;

    S14:
    lastT = Token(OPEN_BRACE, lineNo);
    return lastT;

    S15:
    lastT = Token(COMMA, lineNo);
    return lastT;

    S16:
    lastT = Token(CLOSE_BRACKET, lineNo);
    return lastT;

    S17:
    lastT = Token(CLOSE_BRACE, lineNo);
    return lastT;

    S18:
    lastT = Token(SEMICOLON, lineNo);
    return lastT;

    S19: // String literal handling.
    c = read();
    while(c != '"')
    {
        literal.push_back(c);

        if(c == -1)
            error("String terminated by EOF.", lineNo);
        if(c == '\n')
            error("String contains newline.", lineNo);
        if(!isascii(c))
            warning("Skipping non-ASCII input character.", lineNo);
        else if(c == '\\')
        {
            literal.pop_back();
            c = read();
            if(c == -1)
                error("String terminated by EOF.", lineNo);
            if(c == '\n')
                error("String contains newline.", lineNo);
            switch(c) {
                case 'b':
                    literal.push_back('\b');
                    break;
                case 'f':
                    literal.push_back('\f');
                    break;
                case 'n':
                    literal.push_back('\n');
                    break;
                case 'r':
                    literal.push_back('\r');
                    break;
                case 't':
                    literal.push_back('\t');
                    break;
                case '\\':
                    literal.push_back('\\');
                    break;
                case '"':
                    literal.push_back('"');
                    break;
                default:
                    error("Bad string escape character: '\\" + std::string(1, c) + "'", lineNo);
            }
        }
        c = read();
    }
    lastT = Token(STRING, literal, lineNo);
    return lastT;

    S20:
    while(std::isdigit(c))
    {
        literal.push_back(c);
        c = read();
    }
    unread();
    lastT = Token(INT, literal, lineNo);
    return lastT;

    S21:
    while(c == '_' || std::isalnum(c))
    {
        literal.push_back(c);
        c = read();
    }
    unread();
    // Uses a hashmap to resolve keywords and bool literals.
    if(keywords.count(literal))
        lastT = Token(keywords.at(literal), literal, lineNo);
    else
        lastT = Token(ID, literal, lineNo);
    return lastT;

    S22:
    if(c == '\0')
        warning("Skipping NUL character.", lineNo);
    else if(!isascii(c))
        warning("Skipping non-ASCII input character.", lineNo);
    else if(c == '\b')
        warning("Skipping unknown input character: '\\b'", lineNo);
    else
        warning("Skipping unknown input character: '" + std::string(1, c) + "'", lineNo);
    goto START;

}

