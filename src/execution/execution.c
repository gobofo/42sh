#include "execution.h"

extern struct env *env;

static int execute_cmd(char **command);
static int execute_node(struct AST *root);
static int execute_function(struct AST *root, char **command);

struct builtin builtins_table[];

// #############
// #   UTILS   #
// #############

static int my_return(char **command)
{
	env->should_return = 1;
	return atoi(command[0]);
}

// ####################
// #   REDIRECTIONS   #
// ####################

// The recursiv function to execute the various REDIR
int do_redir(struct AST *root, struct AST **redir)
{
	if (redir && *redir)
		return execute_redir(root, redir);

	if (root->rule == AST_SIMPLE_CMD)
	{
		char **command = create_command(root);
		int status = execute_cmd(command);

		return status;
	}

	int status = execute_node(root);

	return status;
}

// ###############################
// #   SIMPLE & SHELL COMMANDS   #
// ###############################

static int execute_cmd(char **command)
{
	if (!command || command[0] == NULL)
	{
		if (command)
			free(command);

		return 0;
	}

	int status = 0;

	for (int i = 0; builtins_table[i].name != NULL; i++)
	{
		if (strcmp(command[0], builtins_table[i].name) == 0)
		{
			if (strcmp(command[0], ".") == 0)
				status = builtins_table[i].func(command);
			else
				status = builtins_table[i].func(command + 1);

			fflush(stdout);

			goto clean_up;
		}
	}

	struct AST* func;
	
	if ((func = hash_map_get(env->functions, command[0])))
	{
		status = execute_function(func, command + 1);
		goto clean_up;
	}

	status = execute_non_builtin(command);

clean_up:

	for (int i = 0; command[i] != NULL; i++)
		free(command[i]);

	free(command);

	env->last_exit_code = status;

	return status;
}

static int execute_simple_cmd(struct AST *root)
{
	if (env->should_exit == 1)
		return env->last_exit_code;

	// In a command, some variables can be assigned but only with some specific
	// conditions: THERE CAN BE ONLY ASSIGNMENTS IN THE COMMAND
	int is_assignment = 1;

	for (int i = 0; i < root->count_children; i++)
	{
		if (root->children[i]->rule != AST_ASSIGNEMENT)
		{
			is_assignment = 0;
			break;
		}
	}

	// We have only assignments to execute
	if (is_assignment == 1)
	{
		int status = 0;
		for (int i = 0; i < root->count_children; i++)
			status = variable_assignation(root->children[i]);

		return status;
	}

	struct AST **redir = create_redir(root);

	int status = do_redir(root, redir);

	free(redir);

	return status;
}

static int execute_shell_cmd(struct AST *root)
{
	if (env->should_exit == 1)
		return env->last_exit_code;

	struct AST **redir = create_redir(root);

	int status = do_redir(root->children[0], redir);

	free(redir);

	return status;
}

// ##################
// #   CONDITIONS   #
// ##################

static int execute_if(struct AST *root)
{
	if (env->should_exit == 1)
		return env->last_exit_code;

	int condition = execute_node(root->children[0]);

	int status = 0;

	if (condition == 0)
		status = execute_node(root->children[1]);
	else if (root->count_children > 2)
		status = execute_node(root->children[2]);

	return status;
}

// ############
// #   LOOP   #
// ############

static int execute_while(struct AST *root)
{
	env->boucle_count++;
	if (env->should_exit == 1)
		return env->last_exit_code;

	int status = 0;

	while (!execute_node(root->children[0]) && env->break_count == 0 && env->should_return == 0)
	{
		status = execute_node(root->children[1]);

		if(env->continue_count != 0) {
			env->continue_count--;
			if(env->continue_count != 0) {
				break;
			}
		}

		if (env->break_count > 0 || env->should_return != 0) {
			break;
		}
	}

	if (env->break_count)
		env->break_count--;

	env->boucle_count--;
	return status;
}

static int execute_until(struct AST *root)
{
	env->boucle_count++;
	if (env->should_exit == 1)
		return env->last_exit_code;

	int status = 0;

	while (execute_node(root->children[0]) && env->break_count == 0 && env->should_return == 0)
	{
		status = execute_node(root->children[1]);
		if(env->continue_count != 0) {
			env->continue_count--;
			if(env->continue_count != 0) {
				break;
			}
		}
		if (env->break_count > 0 || env->should_return != 0) {
			break;
		}

	}
	if (env->break_count)
		env->break_count--;

	env->boucle_count--;
	return status;
}

static char **create_for_args(struct AST *root)
{
	char **args = calloc(1, sizeof(char *));

	size_t idx = 0;

	for (int i = 1; i < root->count_children - 1; i++)
	{
		char **expanded_values = expand(root->children[i]->content);
		if (expanded_values == NULL)
			continue;

		for (int j = 0; expanded_values[j] != NULL; j++)
		{
			if (expanded_values[j][0] != '\0'
					|| strcmp(root->children[i]->content, "''") == 0
					|| strcmp(root->children[i]->content, "\"\"") == 0)
			{
				args = realloc(args, sizeof(char *) * (idx + 2));
				args[idx++] = expanded_values[j];
				args[idx] = NULL;
			}
			else
			{
				free(expanded_values[j]);
			}
		}

		free(expanded_values);
	}

	return args;
}

static int execute_for(struct AST *root)
{
	env->boucle_count++;
	char *var = root->children[0]->content;

	// Check if the identifier as a valid name
	if (is_valid_name(var) == 0)
	{
		fprintf(stderr, "Error: %s: not a valid identifier\n", var);

		env->should_exit = 1;
		env->last_exit_code = 1;
	}

	if (env->should_exit == 1)
		return env->last_exit_code;

	int exit_code = 0;

	char **args = create_for_args(root);

	// The first children is name of the identifier so we go from the second to
	// the before last child, the last beeing the command to execute inside
	for (int i = 0; args[i] != NULL; i++)
	{
		hash_map_insert(env->variables, var, strdup(args[i]), free);

		exit_code = execute_node(root->children[root->count_children - 1]);

		if(env->continue_count != 0) {
			env->continue_count--;
			if(env->continue_count != 0) {
				break;
			}
		}
		if (env->break_count > 0 || env->should_return != 0) {
			break;
		}

	}

	if (args)
	{
		for (int i = 0; args[i] != NULL; i++)
			free(args[i]);

		free(args);
	}

	if (env->break_count > 0)
		env->break_count--;

	env->boucle_count--;
	return exit_code;
}


// #################
// #   OPERATORS   #
// #################

static int execute_or(struct AST *root)
{
	if (env->should_exit == 1)
		return env->last_exit_code;

	int status = execute_node(root->children[0]);
	if (status != 0)
		return execute_node(root->children[1]);

	return status;
}

static int execute_and(struct AST *root)
{
	if (env->should_exit == 1)
		return env->last_exit_code;

	int status = execute_node(root->children[0]);
	if (status == 0)
		return execute_node(root->children[1]);

	return status;
}

// ############
// #   LIST   #
// ############

static int execute_list(struct AST *root)
{
	if (env->should_exit == 1)
		return env->last_exit_code;

	int status = 0;

	for (int i = 0; i < root->count_children && env->break_count == 0 && env->continue_count==0 && env->should_return==0;i++)
		status = execute_node(root->children[i]);

	return status;
}

// ################
// #   PIPELINE   #
// ################

static pid_t exec_fork(struct AST *root, int intput_pipe, int output_pipe)
{
	pid_t pid = fork();

	if (pid != 0)
		return pid;

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
			fprintf(stderr, "Error: dup2\n");
			return 1;
		}

		close(output_pipe);
	}

	_exit(execute_node(root));
}

static int execute_pipeline(struct AST *root)
{
	if (env->should_exit == 1)
		return env->last_exit_code;

	if (root->count_children == 1)
	{
		if (root->is_neg)
			return !execute_node(root->children[0]);

		return execute_node(root->children[0]);
	}

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
			intput_pipe = last_output;

		if (i != root->count_children - 1)
			output_pipe = fd[1];

		tab_pid[i] = exec_fork(root->children[i], intput_pipe, output_pipe);
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

	if (root->is_neg)
		return !res;

	return res;
}
// ################
// #   SUBSHELL   #
// ################

static int  execute_subshell(struct AST *root){
	pid_t pid =fork();

	if(pid==0){
		_exit(execute_node(root->children[0]));
	}
	int wstatus;
	waitpid(pid, &wstatus, 0);
	int res = WEXITSTATUS(wstatus);
	return res;
}


// ####################
// #    FUNCTIONS     #
// ####################

// This function is called when we encounter a function node
static int create_function(struct AST *root)
{
	hash_map_insert(env->functions, root->content, dup_ast(root), destroy_AST_void);	
	return 0;
}


static int execute_function(struct AST *root, char **command)
{
	struct AST **redir = create_redir(root);

	char **save_argv = env->argv;

	int save_argc = env->argc;

	env->argv = calloc(10, sizeof(char*));
	if(save_argv)
		env->argv[0] = save_argv[0];
	env->argc = 0;

	for(int i = 0; i<8 && command[i]; i++){
		env->argv[i+1] = command[i];
		env->argc++;
	}

	struct AST *func = dup_ast(root);

	int status = do_redir(func->children[0], redir);
	
	env->should_return = 0;

	destroy_AST(func);
	free(env->argv);
	env->argv = save_argv;
	env->argc = save_argc;

	free(redir);

	return status;
}

//###################
//#   LOOKUP TABLES #
//###################

// We use lookup tables to execute the nodes and builtins in a more clear way.
// It avoids a lot of if else clauses or switch cases.
// It makes the code more readable and its more easy to implement some extra
// nodes or builtins to execute.

// Helps to reference each builtin to its corresponding func
struct builtin builtins_table[] =
{
	{"true", my_true},
	{"false", my_false},
	{"echo", my_echo},
	{"cd", my_cd},
	{"exit", my_exit},
	{"break", my_break},
	{"continue", my_continue},
	{"unset", my_unset},
	{"export", my_export},
	{"return", my_return},
	{".", my_dot},
	{NULL, NULL}
};

// Helps to reference each type of node to its corresponding func
int (*execute_node_table[])(struct AST *) =
{
	[AST_LIST] = execute_list,
	[AST_SIMPLE_CMD] = execute_simple_cmd,
	[AST_SHELL_CMD] = execute_shell_cmd,
	[AST_IF] = execute_if,
	[AST_WHILE] = execute_while,
	[AST_UNTIL] = execute_until,
	[AST_FOR] = execute_for,
	[AST_AND] = execute_and,
	[AST_OR] = execute_or,
	[AST_PIPELINE] = execute_pipeline,
	[AST_SUB] = execute_subshell,
	[AST_FUNC] = create_function
};

// ####################
// #   GENERAL NODE   #
// ####################

static int execute_node(struct AST *root)
{
	if (!root)
			return 0;

	if (env->should_exit == 1)
		return env->last_exit_code;

	if (root->rule != AST_VALUE && root->rule != AST_REDIR
			&& root->rule != AST_ASSIGNEMENT)
	{
		int status = execute_node_table[root->rule](root);

		env->last_exit_code = status;

		return status;
	}

	return 0;
}

int execute_ast(struct AST *root)
{
	if (!root)
		return 1;

	env->break_count = 0;
	env->boucle_count=0;
	env->continue_count=0;

	return execute_node(root);
}
