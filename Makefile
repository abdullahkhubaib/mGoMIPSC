SOURCES = main.cpp error.cpp scanner.cpp Token.cpp Node.cpp parser.cpp SymbolTable.cpp semantic.cpp gen.cpp
CPPC = g++
CPPFLAGS = -c -Wall -O2
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = go

all: $(TARGET)

error.o: error.h
Token.o: Token.h
scanner.o: scanner.h
Node.o: Node.h
parser.o: parser.h
SymbolTable.o: SymbolTable.h
semantic.o: semantic.h
gen.o: gen.h
%.o : %.c
$(OBJECTS): Makefile

.cpp.o:
	$(CPPC) $(CPPFLAGS) $< -o $@

$(TARGET): $(OBJECTS)
	$(CPPC) -o $@ $(OBJECTS) $(LDLIBS)

.PHONY: clean
clean:
	rm -f *~ *.o *.s $(TARGET)

