#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <string.h>

#include "../token.h"

struct node
{
    struct token token;
    struct node *next;
};

struct node *lexer(FILE *file);

void free_nodes(struct node *node);

// Handle IO Entries
// All must be a FILE

#endif /* ! LEXER_H */
