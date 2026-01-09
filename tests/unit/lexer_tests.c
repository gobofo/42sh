#define _POSIX_C_SOURCE 200809L

#include <criterion/criterion.h>
#include <criterion/redirect.h>

#include "../../src/lexer/lexer.h"

const char *get_type(enum types type)
{
    switch (type)
    {
    case IF:
        return "IF";
    case THEN:
        return "THEN";
    case ELIF:
        return "ELIF";
    case ELSE:
        return "ELSE";
    case FI:
        return "FI";
    case SEMICOLON:
        return "SEMICOLON";
    case NEWLINE:
        return "NEWLINE";
    case S_QUOTE:
        return "S_QUOTE";
    case WORDS:
        return "WORDS";
    default:
        return "UNKNOWN";
    }
}

void print_lexing(const char *input)
{
    if (!input) return;

    FILE *stream = fmemopen((void*)input, strlen(input), "r");
    if (!stream) return;

    struct token *tok = get_token(stream);
    while (tok != NULL)
	{
        printf("[%s](%s)\n", tok->content, get_type(tok->type));
        
        free_token(tok);
        tok = get_token(NULL);
    }
    
    fclose(stream);
}

void setup_stdout(void)
{
    cr_redirect_stdout();
}

Test(lexer_full, check_simple_word, .init = setup_stdout)
{
    print_lexing("a");
    cr_assert_stdout_eq_str("[a](WORDS)\n");
}
