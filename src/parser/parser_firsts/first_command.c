#include "first_command.h"

/* ============ FIRST ============ */

/* Ce fichier contient les first des commands */
/* (command, shell_command, simple_command, funcdec) */

bool first_command(struct token *token)
{
    if (!token)
        return false;
    return first_simple_command(token) || first_shell_command(token)
        || first_funcdec(token);
}

bool first_shell_command(struct token *token)
{
    if (!token)
        return false;
    enum types type = token->type;
    return first_rule_if(token) || first_rule_while(token)
        || first_rule_until(token) || first_rule_for(token)
        || first_rule_case(token)
        || type == L_BRACE
        || type == L_PAREN;
}

bool first_simple_command(struct token *token)
{
    if (!token)
        return false;
    enum types type = token->type;
    return first_prefix(token) || type == WORDS || type == SUBSHELL;
}

bool first_funcdec(struct token *token)
{
    if (!token)
        return false;
    enum types type = token->type;
    return type == WORDS;
}
