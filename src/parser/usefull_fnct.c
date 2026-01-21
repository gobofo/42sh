#include "usefull_fnct.h"

//renvoi true si le mot est un valid word

bool is_valid_word(struct lexer *lexer)
{
    if (lexer->current == NULL)
    {
        return false;
    }

    enum types type = lexer->current->type;

    return type != NEWLINE && type != AND && type != OR && type != SEMICOLON
        && type != PIPE && type != REDIR;
}

//free le token actuelle et renvoi le suivant

struct lexer *eat(struct lexer *lexer)
{
    free(lexer->current);
    return get_token(lexer);
}

//mange les {/n}

void eat_newlines(struct lexer **lexer)
{
    while ((*lexer)->current && (*lexer)->current->type == NEWLINE)
        *lexer = eat(*lexer);
}

struct token *donne_token(struct lexer *lexer){

    return lexer->current;

}

enum types donne_type(struct lexer *lexer){
    return lexer->current->type;
}

char *donne_content(struct lexer *lexer){
    return lexer->current->content;
}