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
static struct token create_token(char *str)
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
 * @brief  		 Empties the current stream and creates a node
 *
 * The stream holds the current of the token we are gonna create
 * We need to save what is inside the stream
 * Then we empty the stream so it can be used for the next token
 *
 * @param stream The stream where the value of the token is stored
 * @param buff	 The buffer holding the value of the token
 * @param size	 The size (in bytes) of the buffer
 *
 * @return A new node
 */
static struct node *flush_stream(FILE **stream, char **buff, size_t *size)
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

/**
 * @brief 		 	Adds a node to the end of the linked list
 *
 * @param tail 	 	The tail of the linked list
 * @param new_node  The new ndoe to add at the end
 */
static void add_node(struct node **tail, struct node *new_node)
{
	(*tail)->next = new_node;
    *tail = new_node;
}

/**
 * @brief			Flush the stream and adds the new node to the end of the
 * 					list
 *
 * @param tail 		The end of the list
 * @param stream	The stream to read from
 * @param buff		The buff where the content for the node is stored
 * @param size		The size of the buffer (in bytes)
 */ 
static void flush_token(struct node **tail, FILE **stream, char **buff, size_t *size)
{
	struct node *new_node = flush_stream(stream, buff, size);
	add_node(tail, new_node);
}

/**
 * @brief 		Creates a list of tokens from an input
 *
 * Takes a stream as an argument and read the content
 * Creates a unique token for each element in the stream and makes a linked list
 * with all the tokens processed
 *
 * @param file	The stream to read from
 *
 * @return Linked List of tokens
 */
struct node *lexer(FILE *file)
{
	// TODO - Add modularity
	// Create auxiliary functions for each case
	// Could reduce number of lines and add clarity

    struct node *head = calloc(sizeof(struct node), 1);
    struct node *tail = head;

	int c;
    
	char *buff = NULL;
    
	size_t size = 0;
    
	FILE *stream = open_memstream(&buff, &size);

	// We read each character in the input one by one
	// Each time we encounter a token delimiter we create a new token
	// Store it in our list of tokens
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

		// A single quote is found
		// In that case, everythin between the 2 single quotes are considered
		// as a single word
		// Iterate until the next single quote marking the closure of the
		// quoting
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

		// A whitespace marks the end of the token
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
