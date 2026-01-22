#define _POSIX_C_SOURCE 200809L
#include <string.h>

#include "execution_utils.h"

extern struct env *env;

/**
 * @brief		Creates a new entry in the hash_map of the environment
 *
 * When a node of type AST_ASSIGNEMENT, we create a new entry to store the new
 * variable or update an already existing one
 *
 * @param root	The AST node
 */

int variable_assignation(struct AST *root)
{
	char *key = strtok(root->content, "=");
	char *value_raw = strtok(NULL, "");

	if (value_raw == NULL)
		value_raw = "";

	char **expanded = expand(value_raw);

	char *value = "";

	if (expanded != NULL && expanded[0] != NULL)
		value = expanded[0];

	hash_map_insert(env->variables, key, strdup(value), free);

	if (expanded)
	{
		for (int i = 0; expanded[i] != NULL; i++)
			free(expanded[i]);

		free(expanded);
	}

	return 0;
}
