# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

ngn/k is a fast K interpreter written in C. K is an array programming language in the APL family. The project is licensed under GNU AGPLv3 and hosted at https://codeberg.org/ngn/k.

## Build Commands

```bash
# Build the interpreter (default target, also runs tests)
make

# Build only the interpreter
make k

# Build as shared library
make libk.so

# Build as static library
make libk.a

# Clean build artifacts
make c
```

The build uses clang by default with options from `opts` (compiler flags) and `lopts` (linker flags). Output binaries go to `o/` directory.

### Windows Build (w64devkit/MINGW64)

```bash
make CC=gcc kw    # Build k.exe for Windows
```

Uses same `opts` as Linux, links with `-lws2_32`. Output is `k.exe`.

## Testing

```bash
# Run all tests
make t

# Individual test suites:
make tu    # Unit tests (t/t.k via t/t.c)
make tr    # REPL tests (t/repltest.k)
make td    # Dyalog compatibility tests (dy/)
make tg    # Code golf tests (g/)
make te    # Project Euler tests (e/)
make ta    # Advent of Code tests (aoc/)
```

Test files use format: `k_expression /expected_output`

## Architecture

### Core Source Files (single-letter naming convention)

- `a.c/a.h` - Core definitions, type system, macros, function declarations
- `g.h` - Generated accessor macros for x/y/z/u variables (from g.k)
- `k.c/k.h` - C API for embedding K in other programs
- `m.c` - Memory management (bucket allocator)
- `p.c` - Parser
- `e.c` - Evaluator/executor
- `v.c` - Verbs (primitive operations)
- `b.c` - Binary operations
- `0.c-4.c` - Numbered verb implementations
- `i.c` - I/O operations
- `r.c` - REPL functionality
- `s.c` - String operations
- `w.c` - WebAssembly support

### Type System

Types are defined in `a.h` via the `T_` macro. Key types:
- `tA` (1) - Generic list
- `tB-tL` (3-7) - Boolean, byte, short, int, long
- `tF` (8) - Float
- `tC` (9) - Character list
- `tS` (10) - Symbol list
- `tM/tm` (11/12) - Dictionary/atom
- `ti-tx` (13-25) - Packed atoms and function types

Values use tagged pointers with type info in high bytes.

### Macro Conventions

The codebase uses extensive macro shorthand:
- `I(x,a...)` - if(x){a;}
- `P(x,a...)` - if(x) return a
- `F(n,a...)` - for loop 0 to n
- `X(a...)` - switch on x's type
- `Y(a...)` - switch on y's type
- `xn, yn` - length of x, y
- `xt, yt` - type of x, y
- `xV, yV` - pointer to data
- `xR, yR` - increment refcount and return

### K Standard Library

Located in `l/`:
- `dt.k` - Date/time functions
- `fmt.k` - Formatting
- `maths.k` - Mathematical functions
- `utf8.k` - UTF-8 encoding

### WebAssembly Build

```bash
make w    # Build wasm version
make h    # Run local HTTP server with wasm version
```

Requires clang-17 and wasm-ld. Output goes to `o/w/`.

## Command-Line Usage

```bash
./k              # Start REPL
./k script.k     # Run a script file
```

There is no `-e` flag for evaluating expressions. The interpreter either runs the REPL (no arguments) or executes a file (first argument).

## Windows Porting Notes

Key differences when porting to Windows:

### File Mode Flags
Windows defines `O_RDONLY`, `O_RDWR` etc. with `_O_BINARY` (0x8000) included. Code checking file modes must use `(fl&3)` instead of `(fl&O_RDWR)` to correctly distinguish read vs write operations.

### 64-bit Pointers
The `m.c` file needs `defined(_WIN64)` added to the `__LP64__` check for correct 64-bit pointer handling on Windows.

### I/O with Empty Symbol
Reading from stdin via empty symbol (`1:` followed by backtick) requires special handling in `fr()` because:
- `o()` correctly returns 0 (stdin) for empty symbol
- But `N()` macro treats 0 as error and returns early
- Fix: add `P(xts&&!xv,frs(0,i,n))` before the `N(o(...))` call to bypass this for empty symbol
