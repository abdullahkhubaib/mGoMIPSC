#include "main.h"

int main(int argc, char *argv[]) {
    if (argc != 2)
        error("Usage: golf <filename>");

    Scanner scanner(argv[1]);
    Parser parser(&scanner);
    semantic checker;

#if 0 // Scanner output
    Token t = scanner.lex();
    while(t.type != EOFILE) {
        std::cout << t.display() << "\n";
        t = scanner.lex();
    }
#else // Run the parser.
    Node* AST = parser.parse();
    gen Gen(AST);
    #if 1 // Run the semantic checker.
    checker.check(AST);
    #endif
    #if 0 // Display AST
        AST->display();
    #else
        std::cout << Gen.generate();
    #endif

#endif
    return 0;
}
