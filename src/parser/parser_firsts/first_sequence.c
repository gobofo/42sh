#include "first_sequence.h"

/* ============ FIRST ============ */

/* Ce fichier contient les first des sequence */
/* (list, and_or, pipeline, compound_list) */

bool first_list(struct token *token)
{
    if (!token)
        return false;
    return first_and_or(token);
}

bool first_and_or(struct token *token)
{
    if (!token)
        return false;
    return first_pipeline(token);
}

bool first_pipeline(struct token *token)
{
    if (!token)
        return false;
    enum types type = token->type;
    return type == NEG || first_command(token);
}

bool first_compound_list(struct token *token)
{
    if (!token)
        return false;
    enum types type = token->type;
    return type == NEWLINE || first_and_or(token);
}
