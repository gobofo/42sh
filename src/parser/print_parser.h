#ifndef PRINT_PARSER_H
#define PRINT_PARSER_H

#include "../ast/ast.h"
#include "../lexer/lexer.h"
#include "../token.h"

struct ast_node
{
	enum rule rule;
	
	char *text;
	char *color;
};

/* ============= PRINT AST ============= */

void parser_print(struct AST *ast);

#endif /* ! PRINT_PARSER_H */

