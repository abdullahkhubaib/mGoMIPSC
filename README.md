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

