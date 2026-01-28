#include "switch_follow.h"
bool follow_rule_case(struct token *token)
{
    return follow_shell_command(token);
}
bool follow_case_clause(struct token *token)
{
    return token && token->type == ESAC;
}
bool follow_case_item(struct token *token)
{
    if (!token)
        return false;
    enum types type = token->type;
    return type == D_SEMICOLON || type == NEWLINE || type == ESAC;
}
