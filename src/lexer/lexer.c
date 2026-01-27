#include "lexer.h"

extern struct env *env;

/**
 * Initialise un lexer avec un fichier d'entrée.
 * Alloue la structure lexer et lit le premier token depuis l'input.
 * Retourne le lexer initialisé ou NULL en cas d'erreur d'allocation.
 */

struct lexer *init_lexer(FILE *input)
{
	struct lexer *lexer = malloc(sizeof(struct lexer));
	if (!lexer)
		return NULL;

	struct input_stack *base_input = malloc(sizeof(struct input_stack));
	if (input == NULL)
	{
		free(lexer);
		return NULL;
	}

	base_input->file = input;
	base_input->alias_name = NULL;
	base_input->next = NULL;

	lexer->stack = base_input;
	lexer->next = NULL;
	lexer->current = NULL;

	get_token(lexer);

	return lexer;
}

void free_lexer(struct lexer *lexer)
{
	if (!lexer)
		return;

	struct input_stack *cur = lexer->stack;
	while (cur)
	{
		struct input_stack *temp = cur;
		cur = cur->next;

		if (temp->alias_name)
		{
			fclose(temp->file);
			free(temp->alias_name);
		}

		free(temp);
	}

	if (lexer->current != NULL)
		free_token(lexer->current);

	if (lexer->next != NULL)
		free_token(lexer->next);

    free(lexer);
}

// ######################
// #   MAIN FUNCTIONS   #
// ######################

/**
 * @brief 			Function to get the current token
 *
 * @param lexer		The struct lexer where everything is stored
 *
 * @return 			Returns the lexer holding the new current token
 */

struct lexer *get_token(struct lexer *lexer)
{	
	struct token *tok = NULL;

	// We look if a look ahead token was read
	// If yes we use it as our current token
	// Else we return the token just read in the input
    if (lexer->next == NULL)
	{
        tok = read_input(lexer->stack->file);
	}
	else
    {
        tok = lexer->next;
        lexer->next = NULL;
    }

	// We read all our current stream and the stack is still not empty after
	if (tok == NULL && lexer->stack->next != NULL)
	{
		struct input_stack *temp = lexer->stack;

		lexer->stack = lexer->stack->next;
		fclose(temp->file);

		if (temp->alias_name != NULL)
			free(temp->alias_name);

		free(temp);
		
		return get_token(lexer);
	}

	// We find an alias
	if (tok != NULL && tok->type == WORDS)
	{
		char *value = hash_map_get(env->alias, tok->content);

		int in_alias = 0;

		// We want to avoid getting in a recursion loop of aliases
		// So if we find an alias containing this alias already we dont expand
		// again this same alias
		for (struct input_stack *s = lexer->stack; s != NULL; s = s->next)
		{
			if(s->alias_name && strcmp(s->alias_name, tok->content) == 0)
				in_alias = 1;
		}

		if (value != NULL && in_alias == 0)
		{
			struct input_stack *new_stream = malloc(sizeof(struct input_stack));

			new_stream->file = fmemopen(value, strlen(value), "r");
			new_stream->alias_name = strdup(tok->content);
			new_stream->next = lexer->stack;

			lexer->stack = new_stream;

			free_token(tok);
			return get_token(lexer);
		}
	}
	
	lexer->current = tok;
    return lexer;
}

/**
 * @brief			Return the look ahead token, the token following the current
 *
 * @param lexer		The struct lexer where everything is stored
 */

void next_token(struct lexer **lexer)
{
	if ((*lexer)->next != NULL)
		free_token((*lexer)->next);

	(*lexer)->next = read_input((*lexer)->stack->file);
}

// ##############
// #   STREAM   #
// ##############

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
	{
        free(*buffer);
		
		*buffer = NULL;
	}

    return new_token;
}

/**
 * @brief 			Puts back the current character and flushes the stream
 *
 * This function is called when we find a deliminter in the input and we were
 * currently reading a token.
 * Since we need both, the current token and the delimiter, for the parsing we
 * need to empty the stream and return the current so then we can process the
 * delim.
 *
 * @param file		The file from which we read the tokens and in which we want
 * 					to put back the delim
 * @param stream	The stream where the current token is held
 * @param buffer	The buffer linked with the stream
 * @param c			The current character read: the delim
 *
 * @return			The token before the delim
 */

static struct token *empty_stream(FILE *file, FILE **stream, char **buffer,
                                  char c)
{
    ungetc(c, file);

    return flush_stream(*stream, buffer);
}

// ######################
// #   TOKEN HANDLING   #
// ######################

/**
 * @brief			Processes the quote token
 *
 * When quotes are found we keep the literal value inside, so we need to
 * iterate on the input until the corresponding closing quote is found.
 *
 * @param file		The file in which we read the input
 * @param stream	The stream holding the token
 * @param c			The start of the quoting token
 */

static void handle_quotes(FILE *file, FILE **stream, int *c)
{
    int open_quote = *c;

    fputc(*c, *stream);

    // While we not find the corresponding closing quote we add each char
    // between the quotes in the stream
    while ((*c = fgetc(file)) != EOF && *c != open_quote)
    {
        // In double quotes, some characters can be escaped with the \ so when
        // one is found we have to take a particular action
        if (*c == '\\' && open_quote == '"')
        {
            int next = fgetc(file);

            if (next != EOF)
            {
                if (next == '$' || next == '`' || next == '"' || next == '\\'
                    || next == '\n')
                {
                    fputc('\\', *stream);
                    fputc(next, *stream);
                }
                else
                {
                    ungetc(next, file);
                }
            }
        }

        fputc(*c, *stream);

        // If we find the closing quote then we indicate it by setting the
        // value of the char to something we know is not possible
        if (*c == open_quote)
        {
            *c = -2;
            return;
        }
    }
}

/**
 * @brief			Process the comment token
 *
 * When a comment symbol is found #, there are two options to handle it:
 * - if there is nothing before it then it is considered as a comment and
 *   everything between the symbol and the next \n is ignored
 * - the symbol is in the middle of a word, then it makes part of the word
 *
 * @param file		The file in which we read the input
 * @param stream	The stream holding the token
 * @param size		The size of the buffer (the token read)
 * @param c			The start of the quoting token
 */

static void hanlde_comments(FILE *file, FILE **stream, size_t *size, int *c)
{
    fflush(*stream);

    if (*size == 0)
    {
        while ((*c = fgetc(file)) != EOF && *c != '\n')
            ;
    }

    if (*c == '\n')
        ungetc(*c, file);
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

static struct token *handle_redir(FILE *file, FILE **stream, char **buffer,
                                  int c)
{
    char buff[4] = { 0 };

    buff[0] = c;

	int next = fgetc(file);

	if (next != EOF)
	{
		buff[1] = next;

		// We have both characters that can make part of a redir.
		// Lets check if we can add a last char to out redir.
		if (is_redir_c(buff[1]))
		{
			// Get the 3rd char
			int third = fgetc(file);
			
			if (third != EOF)
			{
				buff[2] = third; 

				// We have a redir
				if (is_redir(buff))
				{
					fprintf(*stream, "%s", buff);
					return flush_stream(*stream, buffer);
				}

				// Else we put back the third char
				ungetc(buff[2], file);
			}

			buff[2] = '\0';

			// We know our 2 chars are chars that can be found in a REDIR but we
			// dont know if tgt they form a REDIR
			if (is_redir(buff))
			{
				fprintf(*stream, "%s", buff);
				return flush_stream(*stream, buffer);
			}
		}

		// We could not create a 3 or 2 char redir so we try for a single char
		ungetc(buff[1], file);
	}

    buff[1] = '\0';

    if (is_redir(buff))
    {
        fprintf(*stream, "%s", buff);
        return flush_stream(*stream, buffer);
    }

    return NULL;
}

// #####################
// #   INPUT READING   #
// #####################

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
    int in_var = 0;
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

            if (size > 0 && c!='!')
                return empty_stream(file, &stream, &buffer, c);

            // If we were already reading a token, then we need to save
            // the delim, return the current token, then read again the
            // delim So once the token is processed we put back the delim in
            // the file stream. This way it can be read again.
            fputc(c, stream);

            return flush_stream(stream, &buffer);
        }

        if (c == '(' || c == ')' || c == '`')
        {
            fflush(stream);

            if ((c == '(' && size > 0 && buffer[size - 1] == '$') || c == '`')
            {
                int nesting = 0;
				int back_quote = 0;

				if (c == '(')
					nesting++;
				else
					back_quote++;

				if (c == '(')
					fputc(c, stream);
				else
					fputs("$(", stream);

				while ((back_quote > 0 || nesting > 0) && (c = fgetc(file)) != EOF)
				{
					if (c == '(')
						nesting++;
					if (c == ')')
						nesting--;

					if (c == '\\')
					{
						c = fgetc(file);
						
						if (c != '`')
							fputc('\\', stream);
						
						fputc(c, stream);

						continue;
					}

					if (back_quote == 1 && c == '`')
					{
						fputc(')', stream);
						break;
					}

                    fputc(c, stream);
                }

                continue;
            }

            if (size > 0)
                return empty_stream(file, &stream, &buffer, c);

			fputc(c, stream);

            return flush_stream(stream, &buffer);
        }

        if (c == '{' || c == '}')
        {
            fflush(stream);

            if (c == '{' && size > 0 && buffer[size - 1] == '$')
            {
                in_var = 1;
                fputc(c, stream);

                continue;
            }

            if (c == '}' && in_var)
            {
                in_var = 0;
                fputc(c, stream);
                continue;
            }

            if (size > 0)
                return empty_stream(file, &stream, &buffer, c);

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

        if (c == '\'' || c == '"')
        {
            handle_quotes(file, &stream, &c);

            if (c == -2)
                continue;
        }

        if (c == '#')
            hanlde_comments(file, &stream, &size, &c);

        fflush(stream);

        if (('0' <= c && c <= '9' && size == 0) || c == '<' || c == '>')
        {
            // If we find a character potentially identifying a redirection, we
            // read the next characters (until 3 read since the max size of a
            // redir is 3).
            // If we found a valid redirection then we return the token found,
            // else we keep going.
			
			if (is_redir_c(c) && size > 0)
				return empty_stream(file, &stream, &buffer, c);

            struct token *token = handle_redir(file, &stream, &buffer, c);

            if (token)
                return token;
        }

        if (c != EOF && c != '\n')
            fputc(c, stream);
    }

    return flush_stream(stream, &buffer);
}

// #############
// #   OTHER   #
// #############

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
