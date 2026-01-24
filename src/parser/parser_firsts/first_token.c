#include "first_token.h"

/* ============ FIRST ============ */

/* Ce fichier contient les first des token */
/* (prefix, redirection, element) */

bool first_prefix(struct token *token)
{
    if (!token)
        return false;
    enum types type = token->type;
    return first_redirection(token) || type==A_WORDS;
}

bool first_redirection(struct token *token)
{
    if (!token)
        return false;
    return token->type == REDIR;
}

bool first_element(struct token *token)
{
    if (!token)
        return false;
    enum types type = token->type;
    return type==WORDS || first_redirection(token) || type == SUBSHELL;
}


