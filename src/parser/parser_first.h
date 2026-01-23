#ifndef PARSER_FIRST_H
#define PARSER_FIRST_H

#include <stdbool.h>
#include <stddef.h>

#include "../token.h"
#include "parser_first.h"

/* ============= FIRST ============= */

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

#endif /* ! PARSER_FIRST_H */
