#include "lexer/lexer.h"

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

    struct node *head = lexer(stream);

    fclose(stream);

    struct node *current = head;
    int i = 0;

    printf("--- DÉBUT DU LEXING ---\n");
    printf("Input: \"%s\"\n\n", argv[1]);

    while (current != NULL)
    {
        printf("Token [%d]:\n", i);
        printf("  Type    : %s (%d)\n", get_type_name(current->token.type),
               current->token.type);

        printf("  Content : [%s]\n",
               current->token.content ? current->token.content : "NULL");
        printf("-----------------------\n");

        current = current->next;
        i++;
    }

    printf("--- FIN DU LEXING ---\n");


	freeNodes(head);
	return 0;
}
