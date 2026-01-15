#include "execution.h"

int execute_node(struct AST *root);
int execute_list(struct AST *root);

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
            expand(&root->children[i]->content);

            command[idx++] = root->children[i]->content;
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

// ####################
// #   REDIRECTIONS   #
// ####################

int do_redir(char **command, struct AST **redir);

int execute_redir(char **command, struct AST **redir)
{
    int fd = -1;

    char ionumber = redir[0]->children[0]->content[0];
    char *content = strdup(redir[0]->children[0]->content);
    char *tofree = content;

    if (ionumber >= '0' && ionumber <= '9')
    {
        fd = ionumber - '0';
        content++;
    }

    int status = 0;

    // IF FOR EACH REDIR
    if (strcmp(content, ">") == 0 || strcmp(content, ">|") == 0)
        status = redir_replace_in(command, redir, fd == -1 ? 1 : fd);
    else if (strcmp(content, ">>") == 0)
        status = redir_append_in(command, redir, fd == -1 ? 1 : fd);
    else if (strcmp(content, "<") == 0)
        status = redir_read(command, redir, fd == -1 ? 0 : fd);
    else if (strcmp(content, ">&") == 0)
        status = redir_dup(command, redir, fd == -1 ? 1 : fd);
    else if (strcmp(content, "<&") == 0)
        status = redir_dup(command, redir, fd == -1 ? 0 : fd);
    else if (strcmp(content, "<>") == 0)
        status = redir_open(command, redir, fd == -1 ? 0 : fd);

    free(tofree);

    return status;
}

int do_redir(char **command, struct AST **redir)
{
    if (*redir)
        return execute_redir(command, redir);

    return execute_cmd(command);
}

//###############################
//#   SIMPLE & SHELL COMMANDS   #
//###############################

int execute_cmd(char **command)
{
    int status = 0;

    // We consider the quote expansion has been done
    if (strcmp(command[0], "true") == 0)
        status = 0;
    else if (strcmp(command[0], "false") == 0)
        status = 1;
    else if (strcmp(command[0], "echo") == 0)
        status = my_echo(command + 1);
    else
        status = execute_non_builtin(command);

    return status;
}

int execute_simple_cmd(struct AST *root)
{
    char **command = create_command(root);

    struct AST **redir = create_redir(root);

    int status = do_redir(command, redir);

    free(command);
    free(redir);

    return status;
}

int execute_shell_cmd(struct AST *root)
    return execute_node(root->children[0]);

//##################
//#   CONDITIONS   #
//##################

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

//############
//#   LOOP   #
//############

int execute_while(struct AST *root)
{
    int status = 0;

    while (execute_node(root->children[0]))
        status = execute_node(root->children[1]);

    return status;
}

int execute_until(struct AST *root)
{
    int status = 0;

    while (!execute_node(root->children[0]))
        status = execute_node(root->children[1]);

    return status;
}

int execute_for(struct AST *root)
{
    // TODO - Implement the function
    if (root)
        return 0;

    return 1;
}

//#################
//#   OPERATORS   #
//#################

int execute_or(struct AST *root)
{
    return execute_node(root->children[0]) || execute_node(root->children[1]);
}

int execute_and(struct AST *root)
{
    return execute_node(root->children[0]) && execute_node(root->children[1]);
}

//############
//#   LIST   #
//############

int execute_list(struct AST *root)
{
    int status = 0;

    for (int i = 0; i < root->count_children; i++)
        status = execute_node(root->children[i]);
    return status;
}

//################
//#   PIPELINE   #
//################
int execute_pipeline(struct AST *root)
{
    int negation = 0;

    int status = 0;

    for (int i = 0; i < root->count_children; i++)
    {
        negation = root->children[i]->is_neg % 2;

        status = execute_node(root->children[i]);
    }

    if (negation)
        return !status;

    return status;
}

int execute_node(struct AST *root)
{
    switch (root->rule)
    {
    case AST_LIST:
        return execute_list(root);

    case AST_SIMPLE_CMD:
        return execute_simple_cmd(root);

    case AST_IF:
        return execute_if(root);

    case AST_WHILE:
        return execute_while(root);

    case AST_UNTIL:
        return execute_until(root);

    case AST_FOR:
        return execute_for(root);

    case AST_PIPELINE:
        return execute_pipeline(root);

    case AST_OR:
        return execute_or(root);

    case AST_AND:
        return execute_and(root);

    case AST_SHELL_CMD:
        return execute_shell_cmd(root);

    // Not supposed to get there but we never know
    default:
        printf("Probleme\n");
        return 0;
    }
}

int execute_ast(struct AST *root)
{
    if (!root)
        return 1;

    return execute_node(root);
}
