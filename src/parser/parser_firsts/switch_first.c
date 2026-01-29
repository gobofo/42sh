#include "switch_first.h"

bool first_rule_case(struct token *token)
{
    return token && token->type == CASE;
}

bool first_case_item(struct token *token)
{
    if (!token)
        return false;

    // case_item = ['('] WORD ...
    return token->type == L_PAREN || token->type == WORDS;
}

bool first_case_clause(struct token *token)

{
    if (!token)
        return false;

    // case_clause = case_item ...
    return first_case_item(token);
}
