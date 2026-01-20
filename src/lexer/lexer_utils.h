#ifndef LEXER_UTILS_H
#define LEXER_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../token.h"
#include "../utils.h"

// #############
// #   TOKEN   #
// #############

struct token *create_token(char *str);

// ###################
// #   REDIRECTION   #
// ##################

int is_redir(char *str);

// ####################
// #   MISCELLANEOUS   #
// ####################

void unget_str(const char *str, FILE *stream);

#endif /* ! LEXER_UTILS_H */
