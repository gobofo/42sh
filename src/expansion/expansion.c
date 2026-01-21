#define _POSIX_C_SOURCE 200809L

#include "expansion.h"

extern struct env *env;

static void add_word(struct expanded_words *words, char *word)
{
	// Allocate new space for the new word
	words->words = realloc(words->words, sizeof(char *) * (words->count + 2));

	// Add the word
	words->words[words->count++] = word;

	// Mark the end of the list of words
	words->words[words->count] = NULL;
}


// #############
// #   UTILS   #
// #############

// Determines if the char is a special variables
static int is_special_char(char c)
{
    return c == '$' || c == '`' || c == '"' || c == '\\' || c == '\n';
}

// Determines if the char is a digit
static int is_digit(char c)
{
    return c >= '0' && c <= '9';
}

// ##########################
// #   VARIABLE EXPANSION   #
// ##########################

static void expand_at_quoted(struct expansion_context *context)
{
	fclose(context->stream);

	// If we have already a word we add it in the list
	if (*(context->size) > 0)
		add_word(context->words, strdup(*(context->buffer)));

	// We add each argument as a separate word
	for (int i = 0; i < env->argc; i++)
		add_word(context->words, strdup(env->argv[i]));

	// We open the stream for the next words
	context->stream = open_memstream(context->buffer, context->size);
}

/**
 * @brief			Extends a variable if it is considered special
 *
 * Special variables are #, $, ?, @, *, RANDOM, UID and $1..$n.
 * If we found a special variables, we extend it and indicate we found a special
 * variable
 *
 * @param stream	The stream in which we extend the variable
 * @param str		The str we are reading and we want to extend it
 * @param i			The index in the current string
 *
 * @return			Special Variable was found or no (1 or 0)
 */

static int is_special_variable(struct expansion_context *context,
		char *str, size_t *i)
{
    // The ? return the exit code of the last command executed
    if (str[*i] == '?')
    {
        fprintf(context->stream, "%d", env->last_exit_code);
        (*i)++;
        return 1;
    }
    // $ returns the current PID of the process running
    else if (str[*i] == '$')
    {
        fprintf(context->stream, "%d", getpid());
        (*i)++;
        return 1;
    }
    // # returns the number of arguments in the shell
    else if (str[*i] == '#')
    {
        fprintf(context->stream, "%d", env->argc);
        (*i)++;
        return 1;
    }
    // $@ and $* act the same way, they display all the arguments passed in
    // parameter.
    // The only real difference is when the expansion is done when they are
    // quoted:
    // - $* -> Expands to a single string all the args
    // - $@ -> Expands all args into a list of words
    else if (str[*i] == '@' || str[*i] == '*')
    {
		// Handle the case where the @ is quoted
		if (str[*i] == '@' && context->quoted == 1)
			expand_at_quoted(context);
		else
		{
			for (int j = 0; j < env->argc; j++)
			{
				fputs(env->argv[j], context->stream);

				if (j < env->argc - 1)
					fputc(' ', context->stream);
			}
		}

        (*i)++;
        return 1;
    }
    // If the string is a single digit then it makes part of the args passed
    // going from 1 to 9
    else if (is_digit(str[*i]))
    {
        int idx = str[*i] - '0';

        if (idx > 0 && (idx - 1) < env->argc)
            fputs(env->argv[idx - 1], context->stream);

        (*i)++;
        return 1;
    }

    return 0;
}

/**
 * @brief			Expands the variable depending on various factors
 *
 * Variable expansion can be made in two ways:
 * - $name where name is the name of the variable. We consider the name of the
 *   variable until the next $ or we find a non alphanum char
 * - ${name} where we need to expand everything in the {}
 *
 * @param stream	The stream in which we store the expansion
 * @param str		The string in which we read
 * @param i			The index in the string
 */

static void expand_variable(struct expansion_context *context,
		char *str, size_t *i)
{
    // Pass the $ and get to nect character
    (*i)++;

    // Detect if we have to handle a special variable
    if (is_special_variable(context, str, i) == 1)
        return;

    char *var_name;
    size_t var_len;

    FILE *var = open_memstream(&var_name, &var_len);

    // If we find a opening { we need to go to the closing one and expand what
    // we found between
    if (str[*i] == '{')
    {
        // Pass the {
        (*i)++;

        while (str[*i] != '\0' && str[*i] != '}')
            fputc(str[(*i)++], var);

        if (str[*i] == '}')
            (*i)++;
    }
    else
    {
        while (str[*i] != '\0' && (isalnum(str[*i]) || str[*i] == '_'))
            fputc(str[(*i)++], var);
    }

    fclose(var);

    // These also make part of the special variables but are not composed of
    // only a character
    if (strcmp(var_name, "RANDOM") == 0)
        fprintf(context->stream, "%d", rand() % 32768);
    else if (strcmp(var_name, "UID") == 0)
        fprintf(context->stream, "%d", getuid());
    else
    {
        // The variable is not a 'special' one so we had it in environment and
        // update the hasp_map storing the variables
        char *var_value = hash_map_get(env->variables, var_name);

        if (var_value)
            fputs(var_value, context->stream);
    }

    free(var_name);
}

// ##############
// #   EXPAND   #
// ##############

/**
 * @brief 			Reads a string and expands it if needed
 *
 * The main part of the expansion.
 * We need to take in matter various aspects. If we find a variable name we
 * expand it. In case we find some quotes, we delete them from the string
 * because we dont need them.
 * Quotes apport a specific feature, between single quotes we keep the literal
 * value nothing has to be done.
 * Between double quotes, we can escape some special characters thanks to the \
 * character and we can also expand some variables.
 * If nothing has to be expanded then the string stays the same.
 *	We return an array to make the difference between $@ and "$@"
 *
 * @param value		The string to expand
 *
 * @return			The extended string on an array format
 */

char **expand(char *str)
{
    size_t size = 0;

    char *buffer = NULL;

	struct expanded_words words = { .words = NULL, .count = 0 };

    FILE *stream = open_memstream(&buffer, &size);

	struct expansion_context context = { stream, 0, &buffer, &size, &words };

    size_t i = 0;

    // Iterate the string character by character and we take a specific action
    // when we find something to do
    while (str[i] != '\0')
    {
        // When a \ character is found the next char is 'escaped', this means
        // we take the literal value of it.
        // For example:
        // - \$ becomes $ instead of expanding a variable
        // Same for quotes or the \ char
        if (str[i] == '\\')
        {
            i++;

            // We want to skip the \n after an escape char
            if (str[i] == '\n')
            {
                i++;
                continue;
            }

            if (str[i] != '\0')
                fputc(str[i++], context.stream);
        }
        // Single quote is found, so we take the literal value inside
        else if (str[i] == '\'')
        {
            // Pass the opening quote
            i++;

            // We save the character as it is until the closing quote
            while (str[i] != '\0' && str[i] != '\'')
                fputc(str[i++], context.stream);

            if (str[i] == '\'')
                i++;
        }
        // Double quotes, we can proceed to variable expansion or character
        // escaping inside
        else if (str[i] == '"')
        {
            // Pass the opening quote
            i++;

            while (str[i] != '\0' && str[i] != '"')
            {
                // We have a variable extension
                if (str[i] == '$')
                {
                    expand_variable(&context, str, &i);
                    continue;
                }
                // When inside double quotes the \ char does not act like the
                // normal \.
                // It only escapes when followed by $, `, ", \, or <newline>
                else if (str[i] == '\\' && is_special_char(str[i + 1]) == 1)
                {
                    // We skip the \ since it is supposed to escape
                    i++;
                    fputc(str[i++], context.stream);
                    i++;
                }
                else
				{
                    fputc(str[i++], context.stream);
				}
			}

            if (str[i] == '"')
                i++;
        }
        else if (str[i] == '$')
            expand_variable(&context, str, &i);
        else
            fputc(str[i++], context.stream);
    }

	fclose(context.stream);

	if (size > 0 || words.count == 0)
		add_word(&words, strdup(buffer));

    free(buffer);

    return words.words;
}
