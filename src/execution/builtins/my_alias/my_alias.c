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
	if (strcmp(str, "|") == 0 || strcmp(str, "&") == 0 ||
			strcmp(str, ";") == 0 || strcmp(str, "(") == 0 ||
			strcmp(str, ")") == 0 || strcmp(str, "<") == 0 ||
			strcmp(str, ">") == 0 || strcmp(str, " ") == 0 ||
			strcmp(str, "\n") == 0)
	{
		return 0;
	}
    if(strchr(str,'\n')!=NULL || strchr(str,'\t')!=NULL || strchr(str,' ')!=NULL )
      return 0;
	return 1;
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

		size_t key_len = strchr(actual,'=') - actual;
		// We try to find an equal sign to know if we are doing a definition or
		// just a simple print
		if(strchr(actual,'=') == NULL || key_len ==0)
		{
			// No equal was found, so we try to retrieve a value for the alias
			char* value = hash_map_get(env->alias, actual);

			// No value was found so we have an error
			if(value == NULL)
			{
				fprintf(stderr, "Error: alias: %s : not found\n", actual);
				status = 1;
			}
			else
			{
				printf("%s='%s'\n", actual, value);
			}

			continue;
		}

		// We compute the key len from the equal we found to the start

		// We need to expand the value of the key
		char *key_raw = strndup(actual, key_len);
		char **expanded_key = expand(key_raw, 1);
		free(key_raw);

		if (expanded_key == NULL || expanded_key[0] == NULL)
		{
			free(expanded_key);
			continue;
		}
		
		char *key = expanded_key[0];

		for (size_t i = 1; expanded_key[i] != NULL; i++)
			free(expanded_key[i]);
		free(expanded_key);
		
		// Make sure the alias name is valid
		if (is_valid_alias(key) == 0)
		{
			fprintf(stderr, "Error: alias: '%s': invalid alias name\n", key);

			free(key);

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
			char** expanded_value = expand(value, 1);

			free(value);

			if(expanded_value == NULL)
				continue;

			hash_map_insert(env->alias, key, strdup(expanded_value[0]), free);

			free(expanded_value[0]);
			free(expanded_value);
		}

		free(key);
	}

	return status;
}



int my_unalias(char **command)
{

	if(!*command)
	{
		fprintf(stderr, "Error: unalias : no argument given");
		return 2;
	}

	int status = 0;
	for(; *command; command++)
	{
		if(strcmp("-a", *command) == 0)
		{
			hash_map_free(env->alias, free);
			env->alias = hash_map_init(64);
		}
		else
		{
			bool removed = hash_map_remove(env->alias, *command, free);
			if(!removed)
			{
				fprintf(stderr, "Error: unalias : no alias named %s\n", *command);
				status = 1;
			}
		}
	}
	return status;
}
