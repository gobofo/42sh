#include "follow_loop.h"

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





