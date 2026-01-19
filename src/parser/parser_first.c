

#include "../token.h"
#include "parser.h"

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
    return type==NEG || first_command(token);
}

bool first_command(struct token *token)
{
    if (!token)
        return false;
    enum types type = token->type;
    return first_simple_command(token) || first_shell_command(token) || first_funcdec(token);
}
bool first_funcdec(struct token *token){
    if (!token)
        return false;
    enum types type = token->type;
    return type==WORDS;
}

bool first_shell_command(struct token *token)
{
    if (!token)
        return false;
    enum types type = token->type;
    return first_rule_if(token) || first_rule_while(token) || first_rule_until(token) || first_rule_for(token) || 
      type==L_BRACE || type==L_PAREN;
}

bool first_rule_if(struct token *token)
{
    return token && token->type == IF;
}

bool first_rule_while(struct token *token)
{
    return token && token->type == WHILE;
}

bool first_rule_until(struct token *token)
{
    return token && token->type == UNTIL;
}

bool first_rule_for(struct token *token)
{
    return token && token->type == FOR;
}

bool first_else_clause(struct token *token)
{
    if (!token)
        return false;
    enum types type = token->type;
    return type == ELSE || type == ELIF;
}

bool first_compound_list(struct token *token)
{
    if (!token)
        return false;
    enum types type = token->type;
    return type==NEWLINE || first_and_or(token);
}

bool first_simple_command(struct token *token)
{
    if (!token)
        return false;
    enum types type = token->type;
    return first_prefix(token) || type==WORDS;
}

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
    return type==WORDS || first_redirection(token);
}

// follow
//
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
