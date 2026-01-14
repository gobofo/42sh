#ifndef LEXER_H
#define LEXER_H

#define _POSIX_C_SOURCE 200809L

#include <regex.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "utils.h"
#include "../token.h"

struct token *get_token(FILE *input);
struct token *read_input(FILE *file);

void free_token(struct token *token);

#endif /* ! LEXER_H */
