#ifndef EXECUTION_H
#define EXECUTION_H

#include "../parser/parser.h"
#include "../ast/ast.h"

#include <string.h>

void execute_ast(struct AST *root);

#endif /* ! EXECUTION_H */
