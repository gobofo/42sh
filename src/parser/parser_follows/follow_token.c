#include "follow_token.h"

/* ============ FOLLOW ============ */

/* Ce fichier contient les follow des token */
/* (prefix, redirection, element) */

bool follow_prefix(struct token *token)
{
    if (token == NULL)
        return true;
    enum types type = token->type;
    return follow_simple_command(token) || first_prefix(token) || type==WORDS || type == SUBSHELL;
}

bool follow_redirection(struct token *token)
{
    return follow_prefix(token) || follow_command(token) || follow_simple_command(token);
}

bool follow_element(struct token *token)
{
   return follow_simple_command(token) || first_element(token);
}




