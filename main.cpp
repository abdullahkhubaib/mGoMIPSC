#include <iostream>
#include "./include/Token.h"
#include "./include/scanner.h"
#include "./include/Node.h"
#include "./include/parser.h"
#include "./include/semantic.h"
#include "./include/gen.h"

int main(int argc, char *argv[]) {
    if (argc != 2)
        error("Usage: golf <filename>");

    Scanner scanner(argv[1]);
    Parser parser(&scanner);
    semantic checker;

    Node* AST = parser.parse();
    gen Gen(AST);

    checker.check(AST);
    std::cout << Gen.generate();
    return 0;
}
