#include "execution.h"

int execute_node(struct AST *root);
int execute_list(struct AST *root);
int execute_cmd(char **command);

int do_redir(struct AST *root, struct AST **redir);

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

int execute_redir(struct AST * root, struct AST **redir)
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
		status = redir_replace_in(root, redir, fd == -1 ? 1 : fd);
	else if (strcmp(content, ">>") == 0)
		status = redir_append_in(root, redir, fd == -1 ? 1 : fd);
	else if (strcmp(content, "<") == 0)
		status = redir_read(root, redir, fd == -1 ? 0 : fd);
	else if (strcmp(content, ">&") == 0)
		status = redir_dup(root, redir, fd == -1 ? 1 : fd);
	else if (strcmp(content, "<&") == 0)
		status = redir_dup(root, redir, fd == -1 ? 0 : fd);
	else if (strcmp(content, "<>") == 0)
		status = redir_open(root, redir, fd == -1 ? 0 : fd);

	free(tofree);

	return status;
}

int do_redir(struct AST *root, struct AST **redir)
{
	if (*redir)
		return execute_redir(root, redir);


	if(root->rule==AST_SIMPLE_CMD){

		char **command = create_command(root);
		int status = execute_cmd(command);
		free(command);
		return status;
	}

	// root->rule == AST_SHELL_COMMAND
	
	return execute_node(root);
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
	struct AST **redir = create_redir(root);

	int status = do_redir(root, redir);

	free(redir);

	return status;
}

int execute_shell_cmd(struct AST *root)
{

	struct AST **redir = create_redir(root);

	int status = do_redir(root->children[0], redir);

	free(redir);

	return status;
}

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
	int exit_code =0;
	for(int i=1;i<root->count_children-1;i++){ // on va de deuxieme fils a l avant dernier 
											   // set value  variable : root->children[0]= root->children[i];
		exit_code = execute_node(root->children[root->count_children-1]);
	}

	return exit_code;
}

//#################
//#   OPERATORS   #
//#################

int execute_or(struct AST *root)
{
	return !(!execute_node(root->children[0]) || !execute_node(root->children[1]));
}

int execute_and(struct AST *root)
{
	return !(!execute_node(root->children[0]) && !execute_node(root->children[1]));
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

// ################
// #   PIPELINE   #
// ################

static pid_t exec_fork(struct AST *root , int intput_pipe,int output_pipe){
	pid_t pid = fork();
	if(pid !=0){
		return pid;
	}
	if(intput_pipe!=-1){
		if( dup2(intput_pipe, STDIN_FILENO) == -1){
			fprintf(stderr,"Error: dup2\n");
			return 1;
		}
		close(intput_pipe);
	}
	if(output_pipe!=-1){
		if( dup2(output_pipe, STDOUT_FILENO) == -1){
			fprintf(stderr,"Error: dup2\n");
			return 1;

		}
		close(output_pipe);
	}
	exit(execute_node(root));
}


int execute_pipeline(struct AST *root)
{
	int last_output=-1;

	pid_t* tab_pid= malloc(root->count_children*sizeof(pid_t));

	for(int i=0;i<root->count_children;i++){
		int fd[2];
		if(i< root->count_children-1){
			if(pipe(fd) == -1){
				free(tab_pid);
				fprintf(stderr,"Error: pipe\n");
				return 1;
			}
		}
		int intput_pipe=-1;
		int output_pipe=-1;

		if(i!=0){
			intput_pipe=last_output;
		}
		if(i != root->count_children-1){
			output_pipe=fd[1];
		}

		tab_pid[i] = exec_fork(root->children[i],intput_pipe,output_pipe);
		if(last_output !=-1)
			close(last_output);


		if(i < root->count_children -1){
			close(fd[1]);
			last_output=fd[0];
		}


	}
	int wstatus;
	for(int i =0;i<root->count_children;i++){
		waitpid(tab_pid[i],&wstatus,0);
	}
	int res=  WEXITSTATUS(wstatus);
	free(tab_pid);
    if(root->is_neg){
	  return !res;
    }
    return res;
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
