# bfc

A compiler for the bf esoteric programming language. The compiler takes a bf source file and converts it into an assembly source file. A separate assembler and linker is needed (requires the GNU Assembler).

## Compilation

Type `make <ARCH>` but replace `<ARCH>` with your CPU's architecture. On 64 bit arm systems, this us either `arm64` or `amd64`. On 64 bit desktop PCs, this is most likely `x86_64` or `amd64`.

## Running

To compile a bf source file, run `bfc source.bf -o output.asm`. This will compile whatever source file is passed at `source.bf` into an assembly file at `output.asm`.

After compiling into assembly, the program will need to be assembled and linked. bfc requires the GNU Assembler found in the `binutils` package for most operating systems.

Example:

```sh
$ bfc -o hello.asm examples/hello.bf
$ as -o hello.o hello.asm
$ ld -o hello hello.o
$ ./hello
Hello, World!
```
