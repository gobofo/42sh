#ifndef USEFULL_FNCT_H
#define USEFULL_FNCT_H

#include <stdbool.h>
#include <stddef.h>
#include <unistd.h>

#include "../token.h"
#include "../lexer/lexer.h"
#include "../launch_shell.h"

/* ============= USEFULL FONCTION ============= */

struct lexer *eat(struct lexer *lexer);
void eat_newlines(struct lexer **lexer);
struct token *get_current_token(struct lexer *lexer);
enum types get_current_type(struct lexer *lexer);
char *get_current_content(struct lexer *lexer);
bool is_valid_word(struct lexer *lexer);
char *extract_parentheses_content(struct token *token, int *cmpt);
bool my_42sh_verif(int argc, char *argv[]);
bool verif_subshell(struct lexer *lexer);

#endif /* ! USEFULL_FNCT_H */
