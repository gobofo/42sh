#include "lexer.h"
#include <stdlib.h>
#include <ctype.h>

struct token create_token(char *str)
{
    // Cannot be NULL
    // Already checked before

    struct token token;
    token.content = str;

    if (strcmp(str, "if") == 0)
        token.type = IF;
    else if (strcmp(str, "then") == 0)
        token.type = THEN;
    else if (strcmp(str, "elif") == 0)
        token.type = ELIF;
    else if (strcmp(str, "else") == 0)
        token.type = ELSE;
    else if (strcmp(str, "fi") == 0)
        token.type = FI;
    else if (strcmp(str, ";") == 0)
        token.type = SEMICOLON;
    else if (strcmp(str, "\n") == 0)
        token.type = NEWLINE;
    else if (strcmp(str, "'") == 0)
        token.type = S_QUOTE;
    else
        token.type = WORDS;

    return token;
}


// ajoute le token a la liste et reset le stream
void flush_token(struct node **head, struct node **tail, FILE **stream,
                 char **buff, size_t *size)
{
    fclose(*stream);

    // ici on gere les espaces qui s'enchaine ou si on arrive a la fin
    if (*size == 0)
    {
        free(*buff);
        *stream = open_memstream(buff, size);
        return;
    }

    struct node *new_node = calloc(1, sizeof(struct node));
    new_node->token = create_token(*buff);

    if (*head == NULL)
        *head = new_node;
    else
        (*tail)->next = new_node;
    *tail = new_node;

    *buff = NULL;
    *size = 0;
    *stream = open_memstream(buff, size);
}

struct node *lexer(FILE *file)
{
    struct node *head = NULL;
    struct node *tail = NULL;

	// TODO pour eviter le clang-tidy (5 parametre a flush_token) on peut definir la head comme un token vide (mais alloue)
	// et on commence a ajouter a partir de lui
	// puis au moment du return on free head et return l'element suivant (qui est le premier)
	// donc si il n'y a rien on return bien NULL
	// exemple :
	// a l'initialisation : 
	//     (head/tail)->NULL
	// a l'ajout :
	//     (head)->(tail)->NULL
	// encore un ajout :
	//     (head)->(---)->(tail)->NULL
	// et au return on renvoie ce qu'il y a apres le head :
	//     (---)->(tail)->NULL
	//
	// comme ca dans le code on utilise que tail

    char *buff = NULL;
    size_t size = 0;
    FILE *stream = open_memstream(&buff, &size);

    int c;

    while ((c = fgetc(file)) != EOF)
    {
        if (c == '#') // TODO gerer le cas ou # est dans un mot
        {
            while ((c = fgetc(file)) != EOF && c != '\n')
            {
            }
            if (c == EOF)
                break;
        }

        if (c == '\'')
        {
            flush_token(&head, &tail, &stream, &buff, &size);
            fputc(c, stream);
            flush_token(&head, &tail, &stream, &buff, &size);
            while ((c = fgetc(file)) != EOF && c != '\'')
            {
                fputc(c, stream);
            }
            flush_token(&head, &tail, &stream, &buff, &size);
            if (c == EOF)
                break; // TODO erreur a declancher ici si la quote n'est pas
                       // fermee
            fputc(c, stream);
            flush_token(&head, &tail, &stream, &buff, &size);
            continue;
        }

        if (c==' ' || c=='\t')
        {
            flush_token(&head, &tail, &stream, &buff, &size);
            continue;
        }

        if (c == ';' || c=='\n')
        {
            flush_token(&head, &tail, &stream, &buff, &size);
            fputc(c, stream);
            flush_token(&head, &tail, &stream, &buff, &size);
            continue;
        }

        fputc(c, stream);
    }

    flush_token(&head, &tail, &stream, &buff, &size);

    fclose(stream);
    free(buff);

    return head;
}


// TODO fonction pour free
// reste a savoir si on free le champ content de token ou si le parseur l'utilise
