#include "first_loop.h"

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



