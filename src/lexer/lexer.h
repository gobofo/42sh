#ifndef LEXER_H
#define LEXER_H

#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../token.h"
#include "utils.h"

struct token *get_token(FILE *input);

void free_token(struct token *token);

#endif /* ! LEXER_H */
