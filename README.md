# 42sh

A POSIX-compliant shell written in C99 from scratch. Implements lexing, parsing, AST construction and execution - no use of `system()` or `popen()`.

## What it does

- Reads shell input interactively or from a script file
- Tokenizes input into a stream of shell tokens
- Parses tokens into an Abstract Syntax Tree using an LL(1) parser
- Executes the AST: commands, pipelines, redirections, control flow
- Expands variables, handles command substitution
- Manages shell environment and builtins

## Supported features

- Commands, pipelines (`|`), sequences (`;`, `&&`, `||`)
- Redirections (`<`, `>`, `>>`, `<<`, `>&`)
- Control flow: `if/then/else/fi`, `while`, `for`, `case`
- Variable expansion, command substitution (`$(...)`)
- Builtins: `cd`, `echo`, `exit`, `export`, `unset`, `alias`, `break`, `continue`, `.`

## Architecture

```
src/
|-- lexer/              # Tokenization
|   |-- lexer.c         # Token recognition and production
|   |-- lexer_utils.c   # Character classification helpers
|   `-- input_stack.c   # Input source management
|-- parser/             # LL(1) parser -> AST
|   |-- parser_firsts/  # FIRST sets for each grammar rule
|   |-- parser_follows/ # FOLLOW sets for each grammar rule
|   `-- parser_fnct/    # Grammar rule implementations
|-- ast/                # AST node definitions and utilities
|-- execution/          # AST evaluation
|   |-- execution.c     # Node dispatch
|   |-- execute_command/ # Fork + exec
|   `-- builtins/       # Built-in commands
|-- expansion/          # Variable and command substitution
|-- environment/        # Shell variables and environment
|-- hash_map/           # Hash map for variables and aliases
`-- io_backend/         # Input handling (interactive / file)
```

## Build

Requires autotools (`autoconf`, `automake`).

```bash
autoreconf -i
./configure
make
./42sh
```

## Tests

```bash
make check
```

## Team

Baptiste Mathieu - Yann Fernandez Puig - Yann Sarthou - Lenny Taurines

EPITA - Systems programming (ING1)