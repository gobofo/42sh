#include "first_condition.h"

/* ============ FIRST ============ */

/* Ce fichier contient les first des conditions */
/* (rule_if, else_clause) */

bool first_rule_if(struct token *token)
{
    return token && token->type == IF;
}

bool first_else_clause(struct token *token)
{
    if (!token)
        return false;
    enum types type = token->type;
    return type == ELSE || type == ELIF;
}




