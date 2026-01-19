#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>

#include "../ast/ast.h"
#include "../lexer/lexer.h"
#include "../token.h"

struct AST *input(struct token **token);

// print
//
bool first_list(struct token *token);
bool first_and_or(struct token *token);
bool first_pipeline(struct token *token);
bool first_command(struct token *token);
bool first_shell_command(struct token *token);
bool first_rule_if(struct token *token);
bool first_rule_while(struct token *token);
bool first_rule_until(struct token *token);
bool first_rule_for(struct token *token);
bool first_else_clause(struct token *token);
bool first_compound_list(struct token *token);
bool first_simple_command(struct token *token);
bool first_prefix(struct token *token);
bool first_redirection(struct token *token);
bool first_element(struct token *token);
bool first_funcdec(struct token *token);

bool follow_input(struct token *token);
bool follow_list(struct token *token);
bool follow_and_or(struct token *token);
bool follow_pipeline(struct token *token);
bool follow_command(struct token *token);
bool follow_shell_command(struct token *token);
bool follow_rule_if(struct token *token);
bool follow_rule_while(struct token *token);
bool follow_rule_until(struct token *token);
bool follow_rule_for(struct token *token);
bool follow_else_clause(struct token *token);
bool follow_compound_list(struct token *token);
bool follow_simple_command(struct token *token);
bool follow_prefix(struct token *token);
bool follow_redirection(struct token *token);
bool follow_element(struct token *token);

void parser_print(struct AST *ast);
#endif /* ! PARSER_H */
