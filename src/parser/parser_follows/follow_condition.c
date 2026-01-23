#include "follow_condition.h"

bool follow_rule_if(struct token *token)
{
    return follow_shell_command(token);
}

bool follow_else_clause(struct token *token)
{
    return token && token->type == FI;
}


