#ifndef EXECUTION_H
#define EXECUTION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "my_echo.h"
#include "../ast/ast.h"
#include "../parser/parser.h"

int execute_ast(struct AST *root);

#endif /* ! EXECUTION_H */
