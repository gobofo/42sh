#ifndef LEXER_H
#define LEXER_H

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <string.h>

#include "../token.h"

struct node
{
    struct token token;
    struct node *next;
};

struct node *lexer(FILE *file);

struct token *get_token(FILE *input);
struct token *read_input(FILE *file);


// Handle IO Entries
// All must be a FILE

#endif /* ! LEXER_H */
