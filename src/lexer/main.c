#include "token.h"
#include "lexer.h"
#include "lexer_utils.h"

struct type_string_map {
    enum types type;
    const char *name;
};

static const struct type_string_map debug_names[] = {
    { IF, "IF" }, { THEN, "THEN" }, { ELIF, "ELIF" }, { ELSE, "ELSE" },
	{ FI, "FI" },

    { SEMICOLON, "SEMICOLON" }, { NEWLINE, "NEWLINE" },

    { S_QUOTE, "S_QUOTE" }, { D_QUOTE, "D_QUOTE" },

    { REDIR, "REDIR" }, { PIPE, "PIPE" }, { AND, "AND" }, { OR, "OR" },

    { ESC, "ESC" }, { NEG, "NEG" },

    { WHILE, "WHILE" }, { UNTIL, "UNTIL" }, { DO, "DO" }, { DONE, "DONE" },
    { FOR, "FOR" }, { IN, "IN" },

    { WORDS, "WORDS" }, { A_WORDS, "ASSIGNMENT_WORD" },

    { L_BRACE, "L_BRACE" }, { R_BRACE, "R_BRACE" },

    { L_PAREN, "L_PAREN" }, { R_PAREN, "R_PAREN" },

	{ SUBSHELL, "SUBSHELL" },

    { 0, NULL }
};

static const char *get_type(enum types type)
{
    for (int i = 0; debug_names[i].name != NULL; i++) {
        if (debug_names[i].type == type)
            return debug_names[i].name;
    }
    return "UNKNOWN";
}

static void print_lexing(const char *input)
{
    if (!input)
		return;

    FILE *stream = fmemopen((void *)input, strlen(input), "r");
    if (!stream)
		return;

    struct lexer *lexer = init_lexer(stream);
    if (!lexer)
	{
        fclose(stream);
        return;
    }

    while (lexer->current != NULL)
    {
        printf("[%s](%s)\n", lexer->current->content, get_type(lexer->current->type));

		struct token *to_free = lexer->current;
        
        if (get_token(lexer) == NULL || lexer->current == NULL)
		{
			free_token(to_free);
			break;
		}
		
		free_token(to_free);
    }

    free_lexer(lexer);
    fclose(stream);
}

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		fprintf(stderr, "Error: Usage: ./%s [input]\n", argv[0]);
		return 1;
	}

	print_lexing(argv[1]);

	return 0;
}
