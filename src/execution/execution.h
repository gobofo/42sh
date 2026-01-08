#ifndef EXECUTION_H
#define EXECUTION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../ast/ast.h"
#include "../parser/parser.h"

void execute_ast(struct AST *root);

#endif /* ! EXECUTION_H */
