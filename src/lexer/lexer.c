#include "lexer.h"

struct token create_token(char *str)
{
    // Cannot be NULL
    // Already checked before

    struct token token;
    token.content = strdup(str);

    if (strcmp(str, "if") == 0)

        return token;
}
