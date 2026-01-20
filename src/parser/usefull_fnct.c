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

struct lexer *eat(struct lexer *lexer)
{
    free_token(token);
    return get_token(lexer);
}

//mange les {/n}

void eat_newlines(struct lexer **lexer)
{
    while (*lexer->current && (*lexer)->current->type == NEWLINE)
        *lexer = eat(*lexer);
}

struct token *donne_token(struct lexer *lexer){

    return lexer->current;

}

enum type donne_type(struct lexer *lexer){
    return lexer->current->type;
}
