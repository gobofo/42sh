#include "follow_loop.h"

/* ============ FOLLOW ============ */

/* Ce fichier contient les follow des loops */
/* (rule_while, rule_until, rule_for) */

bool follow_rule_while(struct token *token)
{
    return follow_shell_command(token);
}

bool follow_rule_until(struct token *token)
{
    return follow_shell_command(token);
}

bool follow_rule_for(struct token *token)
{
    return follow_shell_command(token);
}





