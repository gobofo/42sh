#include "follow_condition.h"

/* ============ FOLLOW ============ */

/* Ce fichier contient les follow des conditions */
/* (rule_if, else_clause) */

bool follow_rule_if(struct token *token)
{
    return follow_shell_command(token);
}

bool follow_else_clause(struct token *token)
{
    return token && token->type == FI;
}


