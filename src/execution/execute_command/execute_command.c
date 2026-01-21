#define _GNU_SOURCE
#include <unistd.h>
#include "execute_command.h"
#include "../../environment/environment.h"
extern struct env *env;

static char ** get_exported_variable(){
  char** var= malloc((env->export_variables->nb_variables +1)* sizeof(char*));
  int c_var=0;
  int i=0;


  while(i<env->export_variables->nb_variables){
    char* name=env->export_variables->list_variables[i];
    char* val = hash_map_get(env->variables,name);

    if(val!=NULL){
      int n = strlen(name);
      int v = strlen(val);
      char* new_var= malloc(n + 1 + v +1); // name + = + val + \0

      strcpy(new_var,name);
      new_var[n]='=';
      new_var[strlen(name)+1]=0;        // concatene les chaine dans le bon 
      strcat(new_var,val);
      var[c_var++]=new_var;
    }
    i++;
  }
  var[c_var]=NULL;
  return var;
}
static void free_exported_variable(char** list_variable_exp){
  int i=0;
  while(list_variable_exp[i]){
    free(list_variable_exp[i++]);
  }

  free(list_variable_exp);
}

int execute_non_builtin(char **cmd)
{
    pid_t pid = fork();

    if (pid == -1)
    {
        fprintf(stderr, "Error: fork() failure\n");
        return 1;
    }

    if (pid == 0)
    {
        char** list_variable_exp= get_exported_variable();

        if (execvpe(cmd[0], cmd, list_variable_exp) == -1)
        {
            fprintf(stderr, "Error: command not found: %s\n", cmd[0]);
            free_exported_variable(list_variable_exp );
            _exit(127);
        }
        free_exported_variable(list_variable_exp );
    }
    else
    {
        int wstatus;
        int child_pid = waitpid(pid, &wstatus, 0);

        if (child_pid == -1)
        {
            fprintf(stderr, "Error: waitpid() failure\n");
            return 1;
        }

        if (WIFEXITED(wstatus))
            return WEXITSTATUS(wstatus);
    }

    _exit(0);
}
