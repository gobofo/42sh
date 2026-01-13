#include "lexer.h"

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

struct token *empty_stream(FILE *file, FILE **stream, char **buffer, char c)
{
    ungetc(c, file);

    return flush_stream(*stream, buffer);
}

void handle_quotes(FILE *file, FILE **stream, int *c)
{
    fputc(*c, *stream);

    while ((*c = fgetc(file)) != EOF && (*c != '\'' || *c != '"'))
        fputc(*c, *stream);
}

void hanlde_comments(FILE *file, FILE **stream, size_t *size, int *c)
{
    fflush(*stream);

    if (size == 0)
    {
        while ((*c = fgetc(file)) != EOF && *c != '\n')
            ;
    }
}

/**
 * @brief			Mimics the function ungetc but for strings
 *
 * Puts back all characters from a string back into a stream
 *
 * @param str		The string which characters we want to put back
 * @param stream	The stream in which we want to put the characters
 */
void unget_str(const char *str, FILE *stream)
{
	size_t len = strlen(str);

	for (size_t i = 0; i < len; i++)
		ungetc(str[i], stream);
}

/**
 * @brief               Process and tokenize redirection operators
 *
 * Identifies redirection sequences by looking ahead in the input.
 * If a redirection is found while the current buffer contains data, it 
 * flushes the buffer first to maintain correct token boundaries.
 * Else, it flushes the redirection itself as a token.
 *
 * @param file          The input stream where the parser is reading
 * @param stream        The memory stream used to build the current token
 * @param buffer        The string buffer associated with the memory stream
 * @param c             The first character of the potential redirection
 *
 * @return              A pointer to the created token, or NULL if no
 * redirection
 */
struct token *handle_redir(FILE *file, FILE **stream, char **buffer, int c)
{
    char buff[4] = { 0 };

    buff[0] = c;
    buff[1] = fgetc(file);
    buff[2] = fgetc(file);

	int idx = 2;

	while (idx >= 0)
	{
		if (is_redir(buff))
		{
			if (strlen(*buffer) > 0)
			{
				unget_str(buff, file);

				return flush_stream(*stream, buffer);
			}

			fprintf(*stream, "%s", buff);
			return flush_stream(*stream, buffer);
		}

		ungetc(buff[idx], file);
		buff[idx] = '\0';

		idx--;
	}

    return NULL;
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
        if (c == '\\')
        {
            fputc(c, stream);
            c = fgetc(file);
            fputc(c, stream);
            continue;
        }

        // A whitespace marks the end of the token
        if (c == ' ' || c == '\t')
        {
            fflush(stream);

            if (size == 0)
                continue;

            return flush_stream(stream, &buffer);
        }

        // Same as before but those charcacters need to be safed as tokens
        if (c == ';' || c == '\n' || c == '!')
        {
            // Sync the stream
            fflush(stream);

            if (size > 0)
                return empty_stream(file, &stream, &buffer, c);

            // If we were already reading a token, then we need to save
            // the delim, return the current token, then read again the
            // delim So once the token is processed we put back the delim in
            // the file stream. This way it can be read again.
            fputc(c, stream);

            return flush_stream(stream, &buffer);
        }

        // Those two characters are considered as operators if they are doubled
        // For the | it can also be considered as a pipe if it is alone.
        // If one this two charcaters is found we need to check if the next one
        // is also the same character.
        if (c == '|' || c == '&')
        {
            fflush(stream);

            if (size > 0)
                return empty_stream(file, &stream, &buffer, c);

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

            ungetc(next_c, file);
            continue;
        }

        // A quote is found.
        // In that case, everything between the 2 quotes is considered as a
		// single word.
        // Iterate until the next quote is found, marking the closure of the
		// quoting.
		// The expansion will be handled in the execution
        if (c == '\'' || c == '"')
            handle_quotes(file, &stream, &c);

        // If we find comments we dont take them in consideration
        // If the # is in the middle of a word then it makes part of the
        // word
        if (c == '#')
            hanlde_comments(file, &stream, &size, &c);

        fflush(stream);

        if (('0' <= c && c <= '2' && size == 0) || c == '<' || c == '>')
        {
			// If we find a character potentially identifying a redirection, we
			// read the next characters (until 3 read since the max size of a 
			// redir is 3).
			// If we found a valid redirection then we return the token found,
			// else we keep going.
            struct token *token = handle_redir(file, &stream, &buffer, c);

            if (token)
                return token;
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
