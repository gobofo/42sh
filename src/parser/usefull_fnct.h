#ifndef USEFULL_FNCT_H
#define USEFULL_FNCT_H

#include <stdbool.h>
#include <stddef.h>
#include "../token.h"
#include "../lexer/lexer.h"

/* ============= USEFULL FONCTION ============= */

void eat_newlines(struct token **token);
struct token *eat(struct token *token);
bool is_valid_word(struct token *token);

#endif /* ! USEFULL_FNCT_H */
