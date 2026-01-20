#include "usefull_fnct.h"

//renvoi true si le mot est un valid word

bool is_valid_word(struct token *token)
{
    if (token == NULL)
    {
        return false;
    }

    enum types type = token->type;

    return type != NEWLINE && type != AND && type != OR && type != SEMICOLON
        && type != PIPE && type != REDIR;
}

//free le token actuelle et renvoi le suivant

struct token *eat(struct token *token)
{
    free_token(token);
    return get_token(NULL);
}

//mange les {/n}

void eat_newlines(struct token **token)
{
    while (*token && (*token)->type == NEWLINE)
        *token = eat(*token);
}
