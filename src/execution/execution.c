#include "execution.h"

#include "../expansion/expansion.h"

int execute_node(struct AST *root);

/**
 * @brief 		Creates the command with children from a node
 *
 * For a given node of the AST tree of type AST_CMD, creates a string from the
 * command node
 *
 * @param root	The parent root with all children beeing the commands
 *
 * @return		A string for command
 */

char **create_command(struct AST *root)
{
    // Is free by caller
    char **command = calloc(root->count_children + 1, sizeof(char *));

    for (int i = 0; i < root->count_children; i++)
    {
        expand(&root->children[i]->content);
        command[i] = root->children[i]->content;
    }

    return command;
}

int execute_cmd(struct AST *root)
{
    char **command = create_command(root);

    int status = 0;

    // We consider the quote expansion has been done
    if (strcmp(command[0], "true") == 0)
        status = 0;
    else if (strcmp(command[0], "false") == 0)
        status = 1;
    else if (strcmp(command[0], "echo") == 0)
        status = my_echo(command + 1);
    else
        // TODO - Use execvp
        fprintf(stderr, "Error: undefined command: \"%s\"\n", command[0]);
    free(command);

    return status;
}

int execute_if(struct AST *root)
{
    int condition = execute_node(root->children[0]);

    int status = 0;

    if (condition == 0)
        status = execute_node(root->children[1]);
    else if (root->count_children > 2)
        status = execute_node(root->children[2]);

    return status;
}

int execute_list(struct AST *root)
{
    int status = 0;

    for (int i = 0; i < root->count_children; i++)
        status = execute_node(root->children[i]);

    return status;
}

int execute_node(struct AST *root)
{
    switch (root->rule)
    {
    case AST_LIST:
        return execute_list(root);

    case AST_CMD:
        return execute_cmd(root);

    case AST_IF:
        return execute_if(root);

    // Not supposed to get there but we never know
    default:
        return 0;
    }
}

int execute_ast(struct AST *root)
{
    if (!root)
        return 1;

    return execute_node(root);
}
