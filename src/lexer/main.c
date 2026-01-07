#include "lexer.h"

const char *get_type_name(enum types type)
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

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <string_to_lex>\n", argv[0]);
        return 1;
    }

    FILE *stream = fmemopen(argv[1], strlen(argv[1]), "r");
    if (!stream)
    {
        perror("fmemopen failed");
        return 1;
    }
	
	struct token *tok = get_token(stream);

	while (tok != NULL)
	{
		printf("Token: [%s]\n", tok->content);
	
		free_token(tok);

		tok = get_token(NULL);
	}

	free_token(tok);
	fclose(stream);
	return 0;
}
