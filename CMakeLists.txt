cmake_minimum_required(VERSION 3.22)
project(mGoMIPSC)

set(CMAKE_CXX_STANDARD 17)

add_executable(go main.cpp src/Token.cpp include/Token.h src/scanner.cpp include/scanner.h src/error.cpp include/error.h src/Node.cpp include/Node.h src/parser.cpp include/parser.h src/SymbolTable.cpp include/SymbolTable.h src/semantic.cpp include/semantic.h src/gen.cpp include/gen.h)
