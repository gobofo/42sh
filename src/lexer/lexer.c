#include "lexer.h"

#include <ctype.h>
#include <stdlib.h>

/**
 * @brief		Check if the string is a redirection argument
 *
 * Determine if the string passed in parameter is used as a redirection
 * argument by using regex.
 *
 * @param str	The string (content of the token)
 *
 * @return		Success or Fail (1 or 0)
 */
int is_redir(char *str)
{
    return strcmp(str, ">") == 0 || strcmp(str, "<") == 0
        || strcmp(str, ">>") == 0 || strcmp(str, ">&") == 0
        || strcmp(str, "<&") == 0 || strcmp(str, ">|") == 0
        || strcmp(str, "<>") == 0;
}

/**
 * @brief 		Creates a token.
 *
 * The function creates a token from a specific string, that will be the
 * content, and associates the corresponding type.
 *
 * @param str 	The content of the token.
 * 			  	Must not be NULL
 *
 * @return 		Token
 */
static struct token *create_token(char *str)
{
    if (!str || strcmp(str, "") == 0)
        return NULL;

    struct token *token = malloc(sizeof(struct token));
    token->content = str;

    // IF clause
    if (strcmp(str, "if") == 0)
        token->type = IF;
    else if (strcmp(str, "then") == 0)
        token->type = THEN;
    else if (strcmp(str, "elif") == 0)
        token->type = ELIF;
    else if (strcmp(str, "else") == 0)
        token->type = ELSE;
    else if (strcmp(str, "fi") == 0)
        token->type = FI;
    // END OF COMMAND
    else if (strcmp(str, ";") == 0)
        token->type = SEMICOLON;
    else if (strcmp(str, "\n") == 0)
        token->type = NEWLINE;
    // QUOTES
    else if (strcmp(str, "'") == 0)
        token->type = S_QUOTE;
    else if (strcmp(str, "\"") == 0)
        token->type = D_QUOTE;
    // MISC
    else if (is_redir(str) == 1)
        token->type = REDIR;
    else if (strcmp(str, "|") == 0)
        token->type = PIPE;
    else if (strcmp(str, "||") == 0 || strcmp(str, "&&") == 0)
        token->type = OPERATOR;
    else if (strcmp(str, "\\") == 0)
        token->type = ESC;
    // LOOP clause
    else if (strcmp(str, "while") == 0)
        token->type = WHILE;
    else if (strcmp(str, "until") == 0)
        token->type = UNTIL;
    else if (strcmp(str, "do") == 0)
        token->type = DO;
    else if (strcmp(str, "done") == 0)
        token->type = DONE;
    else if (strcmp(str, "for") == 0)
        token->type = FOR;
    else if (strcmp(str, "in") == 0)
        token->type = IN;
    // OTHER
    else
        token->type = WORDS;

    return token;
}

/**
 * @brief  		 	Empties the current stream and creates a node
 *
 * The stream holds the current of the token we are gonna create
 * We need to save what is inside the stream
 * Then we empty the stream so it can be used for the next token
 *
 * @param stream 	The stream where the value of the token is stored
 * @param buffer	The buffer holding the value of the token
 * @param size	 	The size (in bytes) of the bufferer
 *
 * @return 			The token in the stream
 */
static struct token *flush_stream(FILE *stream, char **buffer)
{
    fclose(stream);

    struct token *new_token = create_token(*buffer);

    if (!new_token)
        free(*buffer);

    return new_token;
}

/**
 * @brief 			Function to return the next token
 *
 * Returns the next token found in the stream when asked from the parser
 *
 * @param input		The input where the parser is reading
 *
 * @return 			Next token in stream
 */
struct token *get_token(FILE *input)
{
    static FILE *stream = NULL;

    if (input)
        stream = input;

    if (!stream)
        stream = input;

    struct token *token = read_input(stream);

    return token;
}

/**
 * @brief			Reads the input from user
 *
 * Reads the user's input in order to determine the next token to process by
 * the parser.
 * Iterates character by character the stream (reprensenting the input) and
 * stops when a vaild token is found.
 *
 * @ param file 	The user's input in a stream form
 *
 * @return			The next valid token in the stream
 */
struct token *read_input(FILE *file)
{
    int c;

    char *buffer = NULL;

    size_t size = 0;

    FILE *stream = open_memstream(&buffer, &size);

    // We read each character in the input one by one
    // Each time we encounter a token delimiter we create a new token
    // Store it in our list of tokens
    while ((c = fgetc(file)) != EOF)
    {
        // A whitespace marks the end of the token
        if (c == ' ' || c == '\t')
        {
            fflush(stream);

            if (size == 0)
                continue;

            return flush_stream(stream, &buffer);
        }

        // Same as before but those charcacters need to be safed as tokens
        if (c == ';' || c == '\n' || c == '!' || c == '\\')
        {
            // Sync the stream
            fflush(stream);

            if (size > 0)
            {
                ungetc(c, file);

                return flush_stream(stream, &buffer);
            }
            else
            {
                // If we were already reading a token, then we need to save
                // the delim, return the current token, then read again the
                // delim So once the token is processed we put back the delim in
                // the file stream. This way it can be read again
                fputc(c, stream);

                return flush_stream(stream, &buffer);
            }
        }

        // Those two characters are considered as operators if they are doubled
        // For the | it can also be considered as a pipe if it is alone
        // If one this two charcaters is found we need to check if the next one
        // is also the same character
        if (c == '|' || c == '&')
        {
            fputc(c, stream);

            int next_c = fgetc(file);
			if (next_c == EOF)
				break;

            // A double is found
            if (next_c == c)
            {
                fputc(next_c, stream);

                return flush_stream(stream, &buffer);
            }
            // No double is found and we are in a pipe case
            else if (c == '|')
            {
                // Put back the next character since it is part of a different
                // token than the pipe
                ungetc(next_c, file);

                return flush_stream(stream, &buffer);
            }
        }

        // A single quote is found
        // In that case, everythin between the 2 single quotes are considered
        // as a single word
        // Iterate until the next single quote marking the closure of the
        // quoting
        if (c == '\'' || c == '"')
        {
            fputc(c, stream);

            while ((c = fgetc(file)) != EOF && (c != '\'' || c != '"'))
                fputc(c, stream);
        }

        // If we find comments we dont take them in consideration
        // If the # is in the middle of a word then it makes part of the
        // word
        if (c == '#')
        {
            fflush(stream);

            if (size == 0)
            {
                while ((c = fgetc(file)) != EOF && c != '\n')
                    ;
            }
        }

        if (c != EOF && c != '\n')
            fputc(c, stream);
    }

    return flush_stream(stream, &buffer);
}

/**
 * @brief 	Frees the token
 *
 * @param	The token to free
 */
void free_token(struct token *token)
{
    if (!token)
        return;

    free(token->content);
    free(token);
}
