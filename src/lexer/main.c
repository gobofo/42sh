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
    case D_QUOTE:
        return "D_QUOTE";

    case REDIR:
        return "REDIR";
    case PIPE:
        return "PIPE";
    case OR:
        return "OR";
    case AND:
        return "AND";
    case NEG:
        return "NEG";

    case WHILE:
        return "WHILE";
    case UNTIL:
        return "UNTIL";
    case DO:
        return "DO";
    case DONE:
        return "DONE";

    case FOR:
        return "FOR";
    case IN:
        return "IN";

    case WORDS:
        return "WORDS";
    case A_WORDS:
        return "A_WORDS";
	case L_BRACE:
		return "L_BRACE";
	case R_BRACE:
		return "R_BRACE";
	case L_PAREN:
		return "L_PAREN";
	case R_PAREN:
		return "R_PAREN";
    default:
        return "UNKNOWN";
    }
}

int main(int argc, char **argv)
{
	FILE *stream;
	if(argc==1)
		stream = stdin;

	else if(argc==2)
    	stream = fmemopen(argv[1], strlen(argv[1]), "r");
    
	else if(argc==3)
		stream = fopen(argv[2], "r");

	else
		return 1;

	if (!stream)
    {
        perror("fmemopen failed");
        return 1;
    }

    struct token *tok = get_token(stream);
	struct token* next = next_token(NULL);
    while (tok != NULL)
    {

		printf("token : [%s](%s)\n", tok->content, get_type_name(tok->type));
		
		if(next)
        	printf("next token : [%s](%s)\n", next->content, get_type_name(next->type));
		else
			printf("pas de next token\n");
		printf("============================\n");

		free_token(tok);

        tok = get_token(NULL);
		next = next_token(NULL);
    }

    printf("\n");

    free_token(tok);
    fclose(stream);
    return 0;
}
