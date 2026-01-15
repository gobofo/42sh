#define _POSIX_C_SOURCE 200809L

#include "expansion.h"

extern struct env *env;

char *expand(char **value)
{
	size_t size;

	char *str = *value;
	char *buffer;

	FILE *stream = open_memstream(&buffer, &size);

	size_t i = 0;

	while (str[i] != '\0')
	{
		if (str[i] == '\'')
		{
			// Pass the opening quote
			i++;

			while (str[i] != '\0' && str[i] != '\'')
				fputc(str[i++], stream);

			if (str[i] == '\'')
				i++;

			continue;
		}
		else if (str[i] == '"')
		{
			// Pass the opening quote
			i++;

			while (str[i] != '\0' && str[i] != '"')
			{
				if (str[i] == '$')
				{
					// Pass the $ and get to nect character
					i++;

					char *var_name;
					size_t var_len;

					FILE *var = open_memstream(&var_name, &var_len);	

					if (str[i] == '{')
					{
						// Pass the {
						i++;

						while (str[i] != '\0' && str[i] != '}')
							fputc(str[i++], var);

						if (str[i] == '}')
							i++;
					}
					else
					{
						while(str[i] != '\0'
								&& (isalnum(str[i]) || str[i] == '_'))
							fputc(str[i++], var);
					}

					fclose(var);
					char *var_value = hash_map_get(env->variables, var_name);

					if (var_value)
						fputs(var_value, stream);

					free(var_name);
				}
				else
				{
					fputc(str[i++], stream);
				}
			}

			if (str[i] == '"')
				i++;

			continue;
		}
		else
		{
			fputc(str[i], stream);
		}

		i++;
	}

    fclose(stream);

    return buffer;
}
