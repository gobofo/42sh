#include "follow_sequence.h"

/* ============ FOLLOW ============ */

/* Ce fichier contient les follow des sequence */
/* (list, and_or, pipeline, compound_list) */

bool follow_list(struct token *token)
{
    return token == NULL || token->type == NEWLINE;
}

bool follow_and_or(struct token *token)
{
    if (token == NULL)
        return true;
    enum types type = token->type;
    return type == SEMICOLON || type == NEWLINE || type == THEN || type == DO
        || type == DONE || type == ELSE || type == ELIF || type == FI || type==R_BRACE || type == R_PAREN;
}

bool follow_pipeline(struct token *token)
{
    if (token == NULL)
        return true;
    enum types type = token->type;
    return type == AND || type == OR || type == SEMICOLON || type == NEWLINE
        || type == THEN || type == DO || type == DONE || type == ELSE
        || type == ELIF || type == FI || type==R_BRACE || type == R_PAREN;

}

bool follow_compound_list(struct token *token)
{
    if (token == NULL)
        return false;
    enum types type = token->type;
    return type == THEN || type == DO || type == DONE || type == ELSE
        || type == ELIF || type == FI  || type==R_BRACE || type == R_PAREN;

}












