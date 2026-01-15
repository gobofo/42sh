#include "execution.h"

int execute_node(struct AST *root, struct env *env);
int execute_list(struct AST *root, struct env *env);
int execute_cmd(char **command);

int do_redir(struct AST *root, struct AST **redir, struct env *env);

//#############
//#   UTILS   #
//#############

int variable_assignation(struct AST *root, struct env *env)
{
	char *key = strtok(root->content, "=");
	char *value = strtok(NULL, "=");

	bool updated;
	hash_map_insert(env->variables, key, value, &updated);	

	return 0;
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

char **create_command(struct AST *root, struct env *env)
{
    // Is free by caller
    char **command = calloc(root->count_children + 1, sizeof(char *));

    size_t idx = 0;

    for (int i = 0; i < root->count_children; i++)
    {
        if (root->children[i]->rule == AST_VALUE)
        {
            expand(&root->children[i]->content, env);

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

int execute_redir(struct AST *root, struct AST **redir, struct env *env)
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
        status = redir_replace_in(root, redir, env, fd == -1 ? 1 : fd);
    else if (strcmp(content, ">>") == 0)
        status = redir_append_in(root, redir, env, fd == -1 ? 1 : fd);
    else if (strcmp(content, "<") == 0)
        status = redir_read(root, redir, env, fd == -1 ? 0 : fd);
    else if (strcmp(content, ">&") == 0)
        status = redir_dup(root, redir, env, fd == -1 ? 1 : fd);
    else if (strcmp(content, "<&") == 0)
        status = redir_dup(root, redir, env, fd == -1 ? 0 : fd);
    else if (strcmp(content, "<>") == 0)
        status = redir_open(root, redir, env, fd == -1 ? 0 : fd);

    free(tofree);

    return status;
}

int do_redir(struct AST *root, struct AST **redir, struct env *env)
{
    if (*redir)
        return execute_redir(root, redir, env);

    if (root->rule == AST_SIMPLE_CMD)
    {
        char **command = create_command(root, env);
        int status = execute_cmd(command);
        free(command);
        return status;
    }

    // root->rule == AST_SHELL_COMMAND

    return execute_node(root, env);
}

// ###############################
// #   SIMPLE & SHELL COMMANDS   #
// ###############################

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

int execute_simple_cmd(struct AST *root, struct env *env)
{
    struct AST **redir = create_redir(root);

	if (root->count_children == 1 &&
			root->children[0]->rule == AST_ASSIGNEMENT)
		return variable_assignation(root->children[0], env);

    int status = do_redir(root, redir, env);

    free(redir);

    return status;
}

int execute_shell_cmd(struct AST *root, struct env *env)
{
    struct AST **redir = create_redir(root);

    int status = do_redir(root->children[0], redir, env);

    free(redir);

    return status;
}

// ##################
// #   CONDITIONS   #
// ##################

int execute_if(struct AST *root, struct env *env)
{
    int condition = execute_node(root->children[0], env);

    int status = 0;

    if (condition == 0)
        status = execute_node(root->children[1], env);
    else if (root->count_children > 2)
        status = execute_node(root->children[2], env);

    return status;
}

// ############
// #   LOOP   #
// ############

int execute_while(struct AST *root, struct env *env)
{
    int status = 0;

    while (execute_node(root->children[0], env))
        status = execute_node(root->children[1], env);

    return status;
}

int execute_until(struct AST *root, struct env *env)
{
    int status = 0;

    while (!execute_node(root->children[0], env))
        status = execute_node(root->children[1], env);

    return status;
}

int execute_for(struct AST *root, struct env *env)
{
    int exit_code = 0;
    for (int i = 1; i < root->count_children - 1; i++)
    { // on va de deuxieme fils a l avant dernier
      // set value  variable : root->children[0]= root->children[i];
        exit_code = execute_node(root->children[root->count_children - 1], env);
    }

    return exit_code;
}

// #################
// #   OPERATORS   #
// #################

int execute_or(struct AST *root, struct env *env)
{
    return !(!execute_node(root->children[0], env)
             || !execute_node(root->children[1], env));
}

int execute_and(struct AST *root, struct env *env)
{
    return !(!execute_node(root->children[0], env)
             && !execute_node(root->children[1], env));
}

// ############
// #   LIST   #
// ############

int execute_list(struct AST *root, struct env *env)
{
    int status = 0;

    for (int i = 0; i < root->count_children; i++)
        status = execute_node(root->children[i], env);
    return status;
}

// ################
// #   PIPELINE   #
// ################

static pid_t exec_fork(struct AST *root, struct env *env,
		int intput_pipe, int output_pipe)
{
    pid_t pid = fork();
    if (pid != 0)
    {
        return pid;
    }
    if (intput_pipe != -1)
    {
        if (dup2(intput_pipe, STDIN_FILENO) == -1)
        {
            fprintf(stderr, "Error: dup2\n");
            return 1;
        }
        close(intput_pipe);
    }
    if (output_pipe != -1)
    {
        if (dup2(output_pipe, STDOUT_FILENO) == -1)
        {
            fprintf(stderr, "Err, envor: dup2\n");
            return 1;
        }
        close(output_pipe);
    }
    exit(execute_node(root, env));
}

int execute_pipeline(struct AST *root, struct env *env)
{
    int last_output = -1;

    pid_t *tab_pid = malloc(root->count_children * sizeof(pid_t));

    for (int i = 0; i < root->count_children; i++)
    {
        int fd[2];

        if (i < root->count_children - 1)
        {
            if (pipe(fd) == -1)
            {
                free(tab_pid);
                fprintf(stderr, "Error: pipe\n");
                return 1;
            }
        }

        int intput_pipe = -1;
        int output_pipe = -1;

        if (i != 0)
        {
            intput_pipe = last_output;
        }
        if (i != root->count_children - 1)

        {
            output_pipe = fd[1];
        }

        tab_pid[i] = exec_fork(root->children[i], env, intput_pipe, output_pipe);

        if (last_output != -1)
            close(last_output);

        if (i < root->count_children - 1)
        {
            close(fd[1]);
            last_output = fd[0];
        }
    }

    int wstatus;

    for (int i = 0; i < root->count_children; i++)
        waitpid(tab_pid[i], &wstatus, 0);

    int res = WEXITSTATUS(wstatus);

    free(tab_pid);

    return res;
}

int execute_node(struct AST *root, struct env *env)
{
    switch (root->rule)
    {
    case AST_LIST:
        return execute_list(root, env);

    case AST_SIMPLE_CMD:
        return execute_simple_cmd(root, env);

    case AST_IF:
        return execute_if(root, env);

    case AST_WHILE:
        return execute_while(root, env);

    case AST_UNTIL:
        return execute_until(root, env);

    case AST_FOR:
        return execute_for(root, env);

    case AST_PIPELINE:
        return execute_pipeline(root, env);

    case AST_OR:
        return execute_or(root, env);

    case AST_AND:
        return execute_and(root, env);

    case AST_SHELL_CMD:
        return execute_shell_cmd(root, env);

        // Not supposed to get there but we never know
    default:
        printf("Probleme\n");
        return 0;
    }
}

int execute_ast(struct AST *root, struct env *env)
{
    if (!root)
        return 1;

    return execute_node(root, env);
}
