#ifndef PARSER_FOLLOW_H
#define PARSER_FOLLOW_H

#include <stdbool.h>
#include <stddef.h>

#include "parser_first.h"

/* ============= FOLLOW ============= */

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

#endif /* ! PARSER_FOLLOW_H */
