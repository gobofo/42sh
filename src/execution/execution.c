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

    for (int i = 0; i < root->count_children; i++)
    {
        expand(&root->children[i]->content);
        command[i] = root->children[i]->content;
    }

    return command;
}

int execute_simple_cmd(struct AST *root)
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
        status = execute_non_builtin(command);
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
    return execute_node(root->children[0]) || execute_node(root->children[1]);
}

int execute_and(struct AST *root)
{
    return execute_node(root->children[0]) && execute_node(root->children[1]);
}

// ####################
// #   REDIRECTIONS   #
// ####################

int redir_in(struct AST *root, int fd)
{
    int fd_save = fd;

    int fd_file =
        open(root->children[1]->content, O_CREAT | O_WRONLY | O_TRUNC, 0644);

    if (fd_file == -1)
    {
        fprintf(stderr, "Error: Could not open the file: %s\n",
                root->children[1]->content);
        return 1;
    }

    if (dup2(fd_file, fd) == -1)
    {
        fprintf(stderr, "Error: Could not dup\n");
        return 1;
    }
    close(fd_file);

    int status = execute_node(root->children[0]);

    if (dup2(fd_save, fd) == -1)
    {
        fprintf(stderr, "Error: Could not dup\n");
        return 1;
    }

    return status;
}

int execute_redir(struct AST *root)
{
    int fd = 1;

    if (root->content[0] >= '0' && root->content[0] <= '9')
        fd = root->content[0] - '0';

    // IF FOR EACH REDIR
    return redir_in(root, fd);
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

    case AST_REDIR:
        return execute_redir(root);

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
