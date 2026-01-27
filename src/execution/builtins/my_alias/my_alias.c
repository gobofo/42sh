#include "my_alias.h"

extern struct env *env;

/**
 * @brief		Checks that the alias name is valid
 *
 * @param str	The name of the alias
 *
 * @return		1 or 0 (Succes or Failure)
 */

static int is_valid_alias(char *str)
{
	return strcmp(str, "|") == 1 && strcmp(str, "&") == 1 &&
		   strcmp(str, ";") == 1 && strcmp(str, "(") == 1 &&
		   strcmp(str, ")") == 1 && strcmp(str, "<") == 1 &&
		   strcmp(str, ">") == 1 && strcmp(str, " ") == 1 &&
		   strcmp(str, "\n") == 1 &&
}

/**
 * @brief		Mimics the alias builtin
 *
 * The alias has 'two modes':
 * - one to define an alias, composed of alias WORD=WORD
 * - one to print the alias if it exists alias WORD
 *
 * @param		The command indicating the alias(es) to define
 *
 * @return		Exit code of the builtin
 */

int my_alias(char **command)
{
	int status = 0;

	while(*command)
	{
		char *actual = *command;

		command++;

		// We try to find an equal sign to know if we are doing a definition or
		// just a simple print
		if(strchr(actual,'=') == NULL)
		{
			// No equal was found, so we try to retrieve a value for the alias
			char* value = hash_map_get(env->alias, actual);

			// No value was found so we have an error
			if(value == NULL)
			{
				fprintf(stderr, "Error: alias: %s : not found\n", value);
				status = 1;
			}
			else
			{
				printf("%s='%s'\n", actual, value);
			}

			continue;
		}

		// We compute the key len from the equal we found to the start
		size_t key_len = strchr(actual,'=') - actual;

		// We need to expand the value of the key
		char *key_raw = strndup(actual, key_len);
		char **expanded_key = expand(key_raw);
		free(key_raw);

		if (expanded_key == NULL || expanded_key[0] == NULL)
		{
			continue;
		}
		
		char *key = expanded_key[0];
		free(expanded_key);
		
		// Make sure the alias name is valid
		if (is_valid_alias(key))
		{
			fprintf(stderr, "Error: alias: '%s': invalid alias name\n", key);
			continue;
		}

		actual += key_len + 1;
	
		// If the alias value is in single quotes we take the literal value
		if(actual[0] == '\'')
		{
			// Skip the opening quote
			actual++;

			// We dont wanna take into consideration the closing quote so we
			// dont use it for the total len
			size_t value_len = strlen(actual)-1;

			hash_map_insert(env->alias, key, strndup(actual, value_len), free);
		}
		else 	
		{
			// Else expansion needs to be done
			size_t value_len;

			if(actual[0] == '\"')
			{
				// Skip the opening quote
				actual++;
				// minus one for the closing quote
				value_len = strlen(actual)-1;
			}
			else
			{
				value_len = strlen(actual);
			}

			char *value = strndup(actual, value_len);
			char** list = expand(value);

			free(value);

			if(list == NULL)
				continue;

			hash_map_insert(env->alias, key, strdup(list[0]), free);

			free(list[0]);
			free(list);
		}

		free(key);
	}

	return status;
}
