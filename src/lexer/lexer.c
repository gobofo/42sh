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

	struct input_stack *base_input = init_input_stack(input); 
	if (input == NULL)
	{
		free(lexer);
		return NULL;
	}

	lexer->is_first = 0;
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

	free(lexer->stack);

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
        tok = read_input(lexer);
	}
	else
    {
        tok = lexer->next;
        lexer->next = NULL;
    }

	// We read all our current stream and the stack is still not empty after
	if (tok == NULL && lexer->stack->next != NULL)
	{
		pop_input_stack(&(lexer->stack));
		return get_token(lexer);
	}
	
	lexer->current = tok;
    return lexer;
}

struct lexer *get_alias_token(struct lexer *lexer)
{
	if (lexer->current == NULL)
		return lexer;

	char *value = hash_map_get(env->alias, lexer->current->content);
	if (value == NULL)
		return lexer;

	// We want to avoid getting in a recursion loop of aliases
	// So if we find an alias containing this alias already we dont expand
	// again this same alias

	for (struct input_stack *s = lexer->stack; s != NULL; s = s->next)
	{
		if(s->alias_name &&
				strcmp(s->alias_name, lexer->current->content) == 0)
			return lexer;
	}

	FILE *alias_file = fmemopen(value, strlen(value), "r");
	if (alias_file != NULL)
	{
		push_input_stack(&(lexer->stack), alias_file, lexer->current->content);

		free_token(lexer->current);

		get_token(lexer);

		return get_alias_token(lexer);
	}

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

	(*lexer)->next = read_input(*lexer);
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

static struct token *flush_stream(struct lexer *lexer)
{
    fclose(lexer->stream);

    struct token *new_token = create_token(lexer->buffer);

    if (!new_token)
	{
        free(lexer->buffer);
		
		lexer->buffer = NULL;
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

static struct token *empty_stream(struct lexer *lexer)
{
    ungetc(lexer->c, lexer->stack->file);

    return flush_stream(lexer);
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

static void handle_quotes(struct lexer *lexer)
{
    int open_quote = lexer->c;

    fputc(lexer->c, lexer->stream);

    // While we not find the corresponding closing quote we add each char
    // between the quotes in the stream
    while ((lexer->c = fgetc(lexer->stack->file)) != EOF && lexer->c != open_quote)
    {
        // In double quotes, some characters can be escaped with the \ so when
        // one is found we have to take a particular action
        if (lexer->c == '\\' && open_quote == '"')
        {
            int next = fgetc(lexer->stack->file);

            if (next != EOF)
            {
                if (next == '$' || next == '`' || next == '"' || next == '\\'
                    || next == '\n')
                {
                    fputc('\\', lexer->stream);
                    fputc(next, lexer->stream);
                }
                else
                {
                    ungetc(next, lexer->stack->file);
                }
            }
        }

        fputc(lexer->c, lexer->stream);

        // If we find the closing quote then we indicate it by setting the
        // value of the char to something we know is not possible
        if (lexer->c == open_quote)
        {
            lexer->c = -2;
            return;
        }
    }
}

static void handle_escape(struct lexer *lexer)
{
	fputc(lexer->c, lexer->stream);
	lexer->c = fgetc(lexer->stack->file);
	fputc(lexer->c, lexer->stream);
}

static struct token *handle_ifs(struct lexer *lexer)
{
	fflush(lexer->stream);

	if (lexer->size == 0)
		return NULL;

	return flush_stream(lexer);
}

static struct token *handle_special_token(struct lexer *lexer)
{
	// Sync the stream
	fflush(lexer->stream);

	if (lexer->size > 0 && lexer->c != '!')
		return empty_stream(lexer);

	// If we were already reading a token, then we need to save
	// the delim, return the current token, then read again the
	// delim So once the token is processed we put back the delim in
	// the file stream. This way it can be read again.
	fputc(lexer->c, lexer->stream);

	if(lexer->c == ';')
	{
		// We take the next char to see if we find another ;
		int next = fgetc(lexer->stack->file);

		// We find it so we create a token D_SEMICOLON
		if(next == ';')
			fputc(next, lexer->stream);
		else
			ungetc(next, lexer->stack->file); 
	}

	return flush_stream(lexer);
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

static void handle_comments(struct lexer *lexer)
{
    fflush(lexer->stream);

    if (lexer->size == 0)
    {
        while ((lexer->c = fgetc(lexer->stack->file)) != EOF &&
				lexer->c != '\n')
            ;
    }

    if (lexer->c == '\n')
        ungetc(lexer->c, lexer->stack->file);
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

static struct token *handle_redir(struct lexer *lexer)
{
	if (is_redir_c(lexer->c) && lexer->size > 0)
		return empty_stream(lexer);

    char buff[4] = { 0 };

    buff[0] = lexer->c;

	int next = fgetc(lexer->stack->file);

	if (next != EOF)
	{
		buff[1] = next;

		// We have both characters that can make part of a redir.
		// Lets check if we can add a last char to out redir.
		if (is_redir_c(buff[1]))
		{
			// Get the 3rd char
			int third = fgetc(lexer->stack->file);
			
			if (third != EOF)
			{
				buff[2] = third; 

				// We have a redir
				if (is_redir(buff))
				{
					fprintf(lexer->stream, "%s", buff);
					return flush_stream(lexer);
				}

				// Else we put back the third char
				ungetc(buff[2], lexer->stack->file);
			}

			buff[2] = '\0';

			// We know our 2 chars are chars that can be found in a REDIR but we
			// dont know if tgt they form a REDIR
			if (is_redir(buff))
			{
				fprintf(lexer->stream, "%s", buff);
				return flush_stream(lexer);
			}
		}

		// We could not create a 3 or 2 char redir so we try for a single char
		ungetc(buff[1], lexer->stack->file);
	}

    buff[1] = '\0';

    if (is_redir(buff))
    {
        fprintf(lexer->stream, "%s", buff);
        return flush_stream(lexer);
    }

    return NULL;
}

static void command_sub_helper(struct lexer *lexer)
{
	int nesting = 0;
	int back_quote = 0;

	if (lexer->c == '(')
		nesting++;
	else
		back_quote++;

	if (lexer->c == '(')
		fputc(lexer->c, lexer->stream);
	else
		fputs("$(", lexer->stream);

	while ((back_quote > 0 || nesting > 0) &&
			(lexer->c = fgetc(lexer->stack->file)) != EOF)
	{
		if (lexer->c == '(')
			nesting++;
		if (lexer->c == ')')
			nesting--;

		if (lexer->c == '\\')
		{
			lexer->c = fgetc(lexer->stack->file);

			if (lexer->c != '`')
				fputc('\\', lexer->stream);

			fputc(lexer->c, lexer->stream);

			continue;
		}

		if (back_quote == 1 && lexer->c == '`')
		{
			fputc(')', lexer->stream);
			break;
		}

		fputc(lexer->c, lexer->stream);
	}
}

static struct token *handle_command_block(struct lexer *lexer)
{
	fflush(lexer->stream);

	if (lexer->c == '{' && lexer->size > 0
			&& lexer->buffer[lexer->size - 1] == '$')
	{
		lexer->in_var = 1;
		fputc(lexer->c, lexer->stream);

		return NULL;
	}

	if (lexer->c == '}' && lexer->in_var)
	{
		lexer->in_var = 0;
		fputc(lexer->c, lexer->stream);

		return NULL;
	}

	if (lexer->size > 0)
		return empty_stream(lexer);

	fputc(lexer->c, lexer->stream);

	return flush_stream(lexer);
}

static struct token *handle_command_sub(struct lexer *lexer)
{
	fflush(lexer->stream);

	if ((lexer->c == '(' && lexer->size > 0 &&
				lexer->buffer[lexer->size - 1] == '$') || lexer->c == '`')
	{
		command_sub_helper(lexer);
		return NULL;
	}

	if (lexer->size > 0)
		return empty_stream(lexer);

	fputc(lexer->c, lexer->stream);

	return flush_stream(lexer);
}

static struct token *handle_operators(struct lexer *lexer)
{
	fflush(lexer->stream);

	if (lexer->size > 0)
		return empty_stream(lexer);

	fputc(lexer->c, lexer->stream);

	int next_c = fgetc(lexer->stack->file);
	if (next_c == EOF)
		return NULL;

	// A double is found
	if (next_c == lexer->c)
	{
		fputc(next_c, lexer->stream);

		return flush_stream(lexer);
	}
	// No double is found and we are in a pipe case
	else if (lexer->c == '|')
	{
		// Put back the next character since it is part of a different
		// token than the pipe
		ungetc(next_c, lexer->stack->file);

		return flush_stream(lexer);
	}

	ungetc(next_c, lexer->stack->file);

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

// 52 LINES
struct token *read_input(struct lexer *lexer)
{
    lexer->in_var = 0;
    lexer->size = 0;
    lexer->buffer = NULL;
    lexer->stream = open_memstream(&(lexer->buffer), &(lexer->size));

    // We read each character in the input one by one
    // Each time we encounter a token delimiter we create a new token
    // Store it in our list of tokens
    while ((lexer->c = fgetc(lexer->stack->file)) != EOF)
    {
        if (lexer->c == '\\')
        {
			handle_escape(lexer);
            continue;
        }

        // A whitespace marks the end of the token
        if (lexer->c == ' ' || lexer->c == '\t')
        {
			struct token *tok = handle_ifs(lexer);
			if (tok == NULL)
				continue;
			
			return tok;
        }
        fflush(lexer->stream);
        // Same as before but those charcacters need to be safed as tokens
        if (lexer->c == ';' || lexer->c == '\n' || (lexer->c == '!' && lexer->size==0))
			return handle_special_token(lexer);

        if (lexer->c == '(' || lexer->c == ')' || lexer->c == '`')
        {
			struct token *tok = handle_command_sub(lexer);
			if (tok == NULL)
				continue;

			return tok;
        }

        if (lexer->c == '{' || lexer->c == '}')
        {
			struct token *tok = handle_command_block(lexer);
			if (tok == NULL)
				continue;

			return tok;
        }

        // Those two characters are considered as operators if they are doubled
        // For the | it can also be considered as a pipe if it is alone.
        // If one this two charcaters is found we need to check if the next one
        // is also the same character.
        if (lexer->c == '|' || lexer->c == '&')
        {
			struct token *tok = handle_operators(lexer);
			if (tok != NULL)
				return tok;

            continue;
        }

        if (lexer->c == '\'' || lexer->c == '"')
        {
            handle_quotes(lexer);

            if (lexer->c == -2)
                continue;
        }

        if (lexer->c == '#')
            handle_comments(lexer);

        fflush(lexer->stream);

        if (('0' <= lexer->c && lexer->c <= '9' && lexer->size == 0) || lexer->c == '<' || lexer->c == '>')
        {
            // If we find a character potentially identifying a redirection, we
            // read the next characters (until 3 read since the max size of a
            // redir is 3).
            // If we found a valid redirection then we return the token found,
            // else we keep going.
			struct token *token = handle_redir(lexer);

            if (token)
                return token;
        }

        if (lexer->c != EOF && lexer->c != '\n')
            fputc(lexer->c, lexer->stream);
    }

    return flush_stream(lexer);
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
