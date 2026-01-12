#ifndef LEXER_H
#define LEXER_H

#define _POSIX_C_SOURCE 200809L

#include <regex.h>
#include <stdio.h>
#include <string.h>

#include "../token.h"

struct node *lexer(FILE *file);

struct token *get_token(FILE *input);
struct token *read_input(FILE *file);
void free_token(struct token *token);

// Handle IO Entries
// All must be a FILE

#endif /* ! LEXER_H */
