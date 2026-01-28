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

    char **expanded = expand(value_raw, 1);

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

    return env->last_exit_code;
}

/**
 * @brief 		Creates the command with children from a node
 *
 * For a given node of the AST tree of type AST_SIMPLE_CMD, creates a string
 * from where the children of the node are values, so a valid command
 *
 * @param root	The parent root with all children beeing the commands
 *
 * @return		The command as a string
 */

char **create_command(struct AST *root)
{
    // Is free by caller
    char **command = calloc(root->count_children + 1, sizeof(char *));

    size_t idx = 0;

    for (int i = 0; i < root->count_children; i++)
    {
        if (root->children[i]->rule == AST_VALUE)
        {
            char **expanded_values = expand(root->children[i]->content, 0);

            for (int j = 0; expanded_values[j] != NULL; j++)
            {
                if (expanded_values[j][0] != '\0'
                    || strcmp(root->children[i]->content, "''") == 0
                    || strcmp(root->children[i]->content, "\"\"") == 0)
                {
                    command = realloc(command, sizeof(char *) * (idx + 2));
                    command[idx++] = expanded_values[j];
                    command[idx] = NULL;
                }
                else
                {
                    free(expanded_values[j]);
                }
            }

            free(expanded_values);
        }
    }

    return command;
}

/**
 * @brief 		Creates a list of AST_REDIR from a command
 *
 * For a given node of the AST tree of type AST_SIMPLE_CMD, creates a list of
 * all redirections to execute
 *
 * @param root	The parent root with all children beeing the commands
 *
 * @return		An array of AST_REDIR nodes
 */

struct AST **create_redir(struct AST *root)
{
    // Free by the caller
    struct AST **redir = calloc(root->count_children + 1, sizeof(struct AST *));

    size_t idx = 0;

    for (int i = 0; i < root->count_children; i++)
    {
        if (root->children[i]->rule == AST_REDIR)
            redir[idx++] = root->children[i];
    }

    return redir;
}

// This function is called when we encounter a function node
int create_function(struct AST *root)
{
    hash_map_insert(env->functions, root->content, dup_ast(root),
                    destroy_AST_void);
    return 0;
}
