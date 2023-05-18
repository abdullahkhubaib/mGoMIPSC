SOURCES = main.cpp src/error.cpp src/scanner.cpp src/Token.cpp src/Node.cpp src/parser.cpp src/SymbolTable.cpp src/semantic.cpp src/gen.cpp src/runtime.cpp
CPPC = g++
CPPFLAGS = -c -Wall -O2
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = go

all: $(TARGET)

error.o: include/error.h
Token.o: include/Token.h
scanner.o: include/scanner.h
Node.o: include/Node.h
parser.o: include/parser.h
SymbolTable.o: include/SymbolTable.h
semantic.o: include/semantic.h
gen.o: include/gen.h
runtime.o:include/runtime.h
%.o : %.c
$(OBJECTS): Makefile

.cpp.o:
	$(CPPC) $(CPPFLAGS) $< -o $@

$(TARGET): $(OBJECTS)
	$(CPPC) -o $@ $(OBJECTS) $(LDLIBS)

.PHONY: clean
clean:
	rm -f *~ *.o src/*.o *.s $(TARGET)

