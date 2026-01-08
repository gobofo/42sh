#ifndef PARSER_H
#define PARSER_H

#include "../ast/ast.h"
#include "../lexer/lexer.h"
#include "../token.h"

struct AST *input(struct token **token);
struct AST *list(struct token **token);
struct AST *and_or(struct token **token);
struct AST *pipeline(struct token **token);
struct AST *command(struct token **token);
struct AST *shell_command(struct token **token);
struct AST *rule_if(struct token **token);
struct AST *else_clause(struct token **token);
struct AST *compound_list(struct token **token);
struct AST *simple_command(struct token **token);
struct AST *element(struct token **token);

// print
//

void parser_print(struct AST *ast);
#endif /* ! PARSER_H */
