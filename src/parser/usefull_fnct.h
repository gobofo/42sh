#ifndef USEFULL_FNCT_H
#define USEFULL_FNCT_H

#include <stdbool.h>
#include <stddef.h>
#include "../token.h"
#include "../lexer/lexer.h"

/* ============= USEFULL FONCTION ============= */

struct lexer *eat(struct lexer *lexer);
void eat_newlines(struct lexer **lexer);
struct token *donne_token(struct lexer *lexer);
enum types donne_type(struct lexer *lexer);
char *donne_content(struct lexer *lexer);
bool is_valid_word(struct lexer *lexer);

#endif /* ! USEFULL_FNCT_H */
