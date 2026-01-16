#define _POSIX_C_SOURCE 200809L

#include "expansion.h"

extern struct env *env;

// #############
// #   UTILS   #
// #############
static int is_special_char(char c)
{
    return c == '$' || c == '`' || c == '"' || c == '\\' || c == '\n';
}

static int is_special_variable(FILE *stream, char *str, size_t *i)
{
	if (str[*i] == '?')
	{
		fprintf(stream, "%d", env->last_exit_code);
		(*i)++;
		return 1;
	}
	else if (str[*i] == '$')
	{
		fprintf(stream, "%d", getpid());
		(*i)++;
		return 1;
	}

	return 0;
}

static void expand_variable(FILE *stream, char *str, size_t *i)
{
    // Pass the $ and get to nect character
    (*i)++;

	if (is_special_variable(stream, str, i) == 1)
		return;

    char *var_name;
    size_t var_len;

    FILE *var = open_memstream(&var_name, &var_len);

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
    char *var_value = hash_map_get(env->variables, var_name);

    if (var_value)
        fputs(var_value, stream);

    free(var_name);
}

char *expand(char **value)
{
    size_t size;

    char *str = *value;
    char *buffer;

    FILE *stream = open_memstream(&buffer, &size);

    size_t i = 0;

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

            fputc(str[i++], stream);
        }
        else if (str[i] == '\'')
        {
            // Pass the opening quote
            i++;

            while (str[i] != '\0' && str[i] != '\'')
                fputc(str[i++], stream);

            if (str[i] == '\'')
                i++;
        }
        else if (str[i] == '"')
        {
            // Pass the opening quote
            i++;

            while (str[i] != '\0' && str[i] != '"')
            {
                if (str[i] == '$')
                    expand_variable(stream, str, &i);

                // When inside double quotes the \ char does not act like the
                // normal \.
                // It only escapes when followed by $, `, ", \, or <newline>
                else if (str[i] == '\\' && is_special_char(str[i + 1]) == 1)
                {
                    // We skip the \ since it is supposed to escape
                    i++;
                    fputc(str[i], stream);
                }
                else
                    fputc(str[i++], stream);
            }

			if (str[i] == '"')
				i++;
        }
        else if (str[i] == '$')
        {
            expand_variable(stream, str, &i);
        }
        else
            fputc(str[i++], stream);
    }

    fclose(stream);

    return buffer;
}
