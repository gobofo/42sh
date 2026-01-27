#include "my_alias.h"

extern struct env *env;

int my_alias(char **command)
{
	int status = 0;
	while(*command)
	{
		char *actual = *command;
		command++;

		if(strchr(actual,'=') == NULL)
		{
			char* value = hash_map_get(env->alias, actual);

			if(value==NULL)
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

		size_t key_len = strchr(actual,'=') - actual;


		char *key = strndup(actual, key_len);

		//TODO handle invalid key

		actual += key_len + 1;
		
		if(actual[0]=='\'')
		{
			actual++;

			// minus one for the closing quote
			size_t value_len = strlen(actual)-1;
			hash_map_insert(env->alias, key, strndup(actual, value_len), free);
		}
		else 	
		{
			size_t value_len;
			if(actual[0]=='\"'){
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
			if(!list)
				continue;
			hash_map_insert(env->alias, key, strdup(list[0]), free);
			free(list[0]);
			free(list);
		}
	}
	return status;
}
