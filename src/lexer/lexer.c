#include "lexer.h"
#include <stdlib.h>
#include <ctype.h>

/**
 * @brief Creates a token.
 *
 * The function creates a token from a specific string, that will be the 
 * content, and associates the corresponding type.
 *
 * @param str The content of the token.
 * 			  Must not be NULL
 *
 * @return Token
 */

struct token create_token(char *str)
{
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

/**
 * @brief Adds the token to the list and creates a new empty stream
 *
 * @param tail 	 The tail of the linked list
 * @param stream The stream where the value of the token is stored
 * @param buff	 The buffer holding the value of the token
 * @param size	 The size (in bytes) of the buffer
 */

struct node *flush_stream(FILE **stream, char **buff, size_t *size)
{
	fclose(*stream);

    if (*size == 0)
    {
        free(*buff);
        *stream = open_memstream(buff, size);
        return NULL;
    }

    struct node *new_node = calloc(1, sizeof(struct node));
    new_node->token = create_token(*buff);

	*buff = NULL;
    *size = 0;
    *stream = open_memstream(buff, size);

	return new_node;
}

void add_node(struct node **tail, struct node *new_node)
{
	(*tail)->next = new_node;
    *tail = new_node;
}

void flush_token(struct node **tail, FILE **stream, char **buff, size_t *size)
{
	struct node *new_node = flush_stream(stream, buff, size);
	add_node(tail, new_node);
}

struct node *lexer(FILE *file)
{
    struct node *head = calloc(sizeof(struct node), 1);
    struct node *tail = head;

	int c;
    
	char *buff = NULL;
    
	size_t size = 0;
    
	FILE *stream = open_memstream(&buff, &size);

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
            flush_token(&tail, &stream, &buff, &size);

			fputc(c, stream);
            
			flush_token(&tail, &stream, &buff, &size);
            
			while ((c = fgetc(file)) != EOF && c != '\'')
            {
                fputc(c, stream);
            }
            
			flush_token(&tail, &stream, &buff, &size);
            
			fputc(c, stream);
            
			flush_token(&tail, &stream, &buff, &size);
            
			continue;
        }

        if (c == ' ' || c == '\t')
        {
            flush_token(&tail, &stream, &buff, &size);
            continue;
        }

        if (c == ';' || c == '\n')
        {
            flush_token(&tail, &stream, &buff, &size);
            
			fputc(c, stream);
            
			flush_token(&tail, &stream, &buff, &size);
            continue;
        }

        fputc(c, stream);
    }

    flush_token(&tail, &stream, &buff, &size);

    fclose(stream);
    free(buff);

	struct node *ret = head->next;
	free(head);

    return ret;
}


void free_nodes(struct node *node)
{
	if(!node)
		return;

	free_nodes(node->next);

	free(node->token.content);
	free(node);
}

// TODO fonction pour free
// reste a savoir si on free le champ content de token ou si le parseur l'utilise
