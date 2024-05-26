# mGoMIPSC

Compiler written using the C++ STL for a minimal subset of Go with a MIPS backend. The compiler is designed as a learning project and performance is not the focus.

The compiler if fully handwritten and currently outputs MIPS assembly code to stdout. Which can then be executed using SPIM or MARS.

## Build

To build this, simply run make on a computer with g++ and make installed on linux.

```bash
make
```

## Usage

Usage involves running the compiler with a filename:
```bash
golf <filename>
```

Saving output to a file:
```bash
golf <filename> > <output>
```

Executing generated code:
```bash
spim -file <output>
```

Inputting a file to be read by the executable:
```bash
spim -file <output> < <input>
```

## Defining Variables

Currently there is only one way to create variables and initialization must be performed seperately.
```go
var <identifier> <type>
<identifier> = <value>
```

Variables can be of the 3 following types:
```go
var a int
a = 12

var b string
b = "Dog"

var c bool
c = false
```

Variables can either be created globally or locally within a scope.

## Functions

Functions can only be created in the global scope and must be defined using the following syntax:
```go
func <identifier>(<identifier> <type>, ..., <identifier> <type>) <type> { }
```

The function can take zero or more arguments and can return a value. The return type must be left empty for void functions.
All functions that are not void must return a value of the data type that is provided in the function defintion.
Void functions can only use an empty return.


In order for a program to be considered valid and to be compiled succesfully, it must contain a main function which has no arguments and no return type:
```go
func main() {}
```

This function acts as the entry point for the program.
