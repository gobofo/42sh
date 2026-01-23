#include "parser_follow.h"

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
    return first_prefix(token) || type==WORDS || type == SUBSHELL;
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
    return type==WORDS || first_redirection(token) || type == SUBSHELL;
}




