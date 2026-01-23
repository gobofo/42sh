#include "follow_command.h"

bool follow_command(struct token *token)
{
    if (token == NULL)
        return true;
    enum types type = token->type;
    return type == PIPE || type == AND || type == OR || type == SEMICOLON
        || type == NEWLINE || type == THEN || type == DO || type == DONE
        || type == ELSE || type == ELIF || type == FI || type==R_BRACE || type == R_PAREN;

}

bool follow_shell_command(struct token *token)
{
    if (token == NULL)
        return true;
    enum types type = token->type;
    return type == REDIR || type == PIPE || type == AND || type == OR
        || type == SEMICOLON || type == NEWLINE || type == THEN || type == DO
        || type == DONE || type == ELSE || type == ELIF || type == FI || type==R_BRACE || type == R_PAREN;

}

bool follow_simple_command(struct token *token)
{
    return follow_command(token);
}

bool follow_funcdec(struct token *token)
{
    return follow_shell_command(token);
}





