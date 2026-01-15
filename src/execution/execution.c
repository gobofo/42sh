#define _POSIX_C_SOURCE 200809L
#include "execution.h"


#include "../expansion/expansion.h"



int execute_node(struct AST *root);
int execute_list(struct AST *root);

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

	size_t idx = 0;

	for (int i = 0; i < root->count_children; i++)
	{
		if (root->children[i]->rule == AST_VALUE)
		{
			expand(&root->children[i]->content);

			// Iterate over all children of the command node
			// Create two separate arrays:
			// - One for WORDS and ASSIGMENT WORDS
			// - One for REDIR
			// The array of WORDS and A_WORDS make part of the command to execute
			// The array of REDIR are the redirs to compute
			// -> Compute them recursivly so we can deredir easily

			command[idx++] = root->children[i]->content;
		}
	}

	return command;
}

struct AST **create_redir(struct AST *root)
{
	struct AST **redir = calloc(root->count_children + 1, sizeof(struct AST *));

	size_t idx = 0;

	for (int i = 0; i < root->count_children; i++)
	{
		if (root->children[i]->rule == AST_REDIR)
			redir[idx++] = root->children[i];
	}

	return redir;
}


int execute_cmd(char **command){
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


// ####################
// #   REDIRECTIONS   #
// ####################

int do_redir(char **command, struct AST **redir);

// REDIR >>
int redir_append_in(char **command, struct AST **redir, int fd)
{
	int fd_file =
		open(redir[0]->children[1]->content, O_CREAT | O_WRONLY | O_APPEND, 0644);

	int fd_save = dup(fd);

	if (dup2(fd_file, fd) == -1)
	{
		fprintf(stderr, "Error: Could not dup\n");
		return 1;
	}
	close(fd_file);

	int status = do_redir(command, redir+1);

	if (dup2(fd_save, fd) == -1)
	{
		fprintf(stderr, "Error: Could not dup\n");
		return 1;
	}
	close(fd_save);

	return status;
}

// REDIR > >|
int redir_replace_in(char **command, struct AST **redir, int fd)
{
	int fd_file =
		open(redir[0]->children[1]->content, O_CREAT | O_WRONLY | O_TRUNC, 0644);

	int fd_save = dup(fd);

	if (dup2(fd_file, fd) == -1)
	{
		fprintf(stderr, "Error: Could not dup\n");
		return 1;
	}
	close(fd_file);

	int status = do_redir(command, redir+1);

	if (dup2(fd_save, fd) == -1)
	{
		fprintf(stderr, "Error: Could not dup\n");
		return 1;
	}
	close(fd_save);

	return status;
}


// REDIR <
int redir_read(char **command, struct AST **redir, int fd)
{
	int fd_file =
		open(redir[0]->children[1]->content, O_RDONLY);

	int fd_save = dup(fd);

	if (dup2(fd_file, fd) == -1)
	{
		fprintf(stderr, "Error: Could not dup\n");
		return 1;
	}
	close(fd_file);

	int status = do_redir(command, redir+1);

	if (dup2(fd_save, fd) == -1)
	{
		fprintf(stderr, "Error: Could not dup\n");
		return 1;
	}
	close(fd_save);

	return status;
}


// REDIR >&
int redir_dup(char **command, struct AST **redir, int fd)
{
	char* word = redir[0]->children[1]->content;

	if(strlen(word)!=1 || (word[0]!='-' && ( word[0]<'0' || '9'<word[0] ))){
		fprintf(stderr, "Error: >&: Wrong FD word = '%s'\n", word);
		return 1;
	}
	
	int fd_save = dup(fd);

	if(word[0]=='-'){

		close(fd);
	}

	else{

		int fd_file = word[0] - '0';

		if(fd!=fd_file){
			if(dup2(fd_file, fd) == -1)
			{
				fprintf(stderr, "Error: Could not dup\n");
				return 1;
			}
		close(fd_file);
		}
	}

	int status = do_redir(command, redir+1);

	if (dup2(fd_save, fd) == -1)
	{
		fprintf(stderr, "Error: Could not dup\n");
		return 1;
	}
	close(fd_save);

	return status;
}


// REDIR <>
int redir_open(char **command, struct AST **redir, int fd)
{
	int fd_file =
		open(redir[0]->children[1]->content, O_RDWR | O_CREAT, 0644);

	int fd_save = dup(fd);

	if (dup2(fd_file, fd) == -1)
	{
		fprintf(stderr, "Error: Could not dup\n");
		return 1;
	}
	close(fd_file);

	int status = do_redir(command, redir+1);

	if (dup2(fd_save, fd) == -1)
	{
		fprintf(stderr, "Error: Could not dup\n");
		return 1;
	}
	close(fd_save);

	return status;
}

int execute_redir(char **command, struct AST **redir)
{
	int fd = -1;

	char ionumber = redir[0]->children[0]->content[0];
	char *content = strdup(redir[0]->children[0]->content);
	char *tofree = content;

	if (ionumber >= '0' && ionumber <= '9'){
		fd = ionumber - '0';
		content++;
	}

	int status = 0;

	// IF FOR EACH REDIR
	if(strcmp(content, ">") == 0 || strcmp(content, ">|") == 0)
		status = redir_replace_in(command, redir, fd == -1 ? 1 : fd);
	else if(strcmp(content, ">>") == 0)
		status = redir_append_in(command, redir, fd == -1 ? 1 : fd);
	else if(strcmp(content, "<") == 0)
		status = redir_read(command, redir, fd == -1 ? 0 : fd);
	else if(strcmp(content, ">&") == 0)
		status = redir_dup(command, redir, fd == -1 ? 1 : fd);
	else if(strcmp(content, "<&") == 0)
		status = redir_dup(command, redir, fd == -1 ? 0 : fd);
	else if(strcmp(content, "<>") == 0)
	  status = redir_open(command, redir, fd == -1 ? 0 : fd);
	free(tofree);
	return status;
}


int do_redir(char **command, struct AST **redir){

	if(*redir){
		return execute_redir(command, redir);
	}
	return execute_cmd(command);
}

int execute_simple_cmd(struct AST *root)
{
	char **command = create_command(root);

	struct AST **redir = create_redir(root);

	// REDIR
	int status = do_redir(command, redir);

	free(command);
	free(redir);

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

// ############
// #   LOOP   #
// ############

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

// #################
// #   OPERATORS   #
// #################

int execute_or(struct AST *root)
{
	return !(!execute_node(root->children[0]) || !execute_node(root->children[1]));
}

int execute_and(struct AST *root)
{
	return !(!execute_node(root->children[0]) && !execute_node(root->children[1]));
}


// ################
// #   PIPELINE   #
// ################

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
//
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
