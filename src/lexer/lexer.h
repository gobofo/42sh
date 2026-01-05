#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <string.h>

#include "../token.h"

struct node
{
    struct token;

    struct node *next;
}

struct token
create_token(char *str);
struct node *lexer(FILE *file);

// Handle IO Entries
// All must be a FILE

#endif /* ! LEXER_H */
