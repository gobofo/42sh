#ifndef PRINT_PARSER_H
#define PRINT_PARSER_H

#include "../ast/ast.h"
#include "../lexer/lexer.h"
#include "../token.h"

/* ============= PRINT AST ============= */

void parser_print(struct AST *ast);

#endif /* ! PRINT_PARSER_H */

