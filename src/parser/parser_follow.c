#include "../token.h"
#include "parser_follow.h"


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

bool follow_rule_if(struct token *token)
{
    return follow_shell_command(token);
}

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
bool follow_funcdec(struct token *token)
{
    return follow_shell_command(token);
}

bool follow_else_clause(struct token *token)
{
    return token && token->type == FI;
}

bool follow_compound_list(struct token *token)
{
    if (token == NULL)
        return false;
    enum types type = token->type;
    return type == THEN || type == DO || type == DONE || type == ELSE
        || type == ELIF || type == FI  || type==R_BRACE || type == R_PAREN;

}

bool follow_simple_command(struct token *token)
{
    return follow_command(token);
}

bool follow_prefix(struct token *token)
{
    if (token == NULL)
        return true;
    enum types type = token->type;
    return follow_simple_command(token) || first_prefix(token) || type==WORDS;
}

bool follow_redirection(struct token *token)
{
    return follow_prefix(token) || follow_command(token) || follow_simple_command(token);
}

bool follow_element(struct token *token)
{
   return follow_simple_command(token) || first_element(token);
}

