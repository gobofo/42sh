#define _POSIX_C_SOURCE 200809L
#include <unistd.h>
#include "execute_command.h"
#include "../../environment/environment.h"
extern struct env *env;


/**
 * Recherche le chemin complet d'une commande en parcourant les dossiers de PATH.
 * Si la commande contient déjà un '/', elle est retournée telle quelle.
 * Retourne le chemin complet si trouvé, NULL sinon.
 */
static char *find_command(char *cmd) {
    static char full_path[1024];
    
    if (strchr(cmd, '/'))
        return cmd;

    char *path_env = getenv("PATH");

    if(path_env==NULL){
      return NULL;
    }

    char *path = strdup(path_env);
    char *dir = strtok(path, ":");
    
    while (dir) {
      strcpy(full_path, dir);
      strcat(full_path, "/");
      strcat(full_path, cmd);
      if (access(full_path, X_OK) == 0) {
        free(path);
        return full_path;
      }
      dir = strtok(NULL, ":");
    }
    free(path);
    return NULL;
}

/**
 * Crée un tableau de chaînes contenant les variables exportées au format "NAME=VALUE".
 * Récupère les valeurs depuis la hashmap et construit les chaînes par concaténation.
 * Retourne un tableau terminé par NULL, compatible avec execve.
 */
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
      new_var[strlen(name)+1]=0;        // concatene les chaine
      strcat(new_var,val);
      var[c_var++]=new_var;
    }
    i++;
  }
  var[c_var]=NULL;
  return var;
}

/**
 * Libère la mémoire d'un tableau de variables exportées.
 * Free chaque chaîne "NAME=VALUE" du tableau, puis le tableau lui-même.
 * S'arrête au marqueur NULL de fin de tableau.
 */
static void free_exported_variable(char** list_variable_exp){
  int i=0;
  while(list_variable_exp[i]){
    free(list_variable_exp[i++]);
  }

  free(list_variable_exp);
}


/**
 * Exécute une commande (non-builtin) dans un processus fils.
 * Fork un processus, recherche la commande dans PATH, et l'exécute avec execve.
 * Retourne le code de sortie de la commande ou 127 si la commande n'est pas trouvée.
 */
int execute_non_builtin(char **cmd)
{
    pid_t pid = fork();

    if (pid == -1)
    {
        fprintf(stderr, "Error: fork() failure\n");
        return 1;
    }

    if (pid == 0)// children 
    {
        char** list_variable_exp= get_exported_variable();
        char *cmd_path = find_command(cmd[0]);

        if (!cmd_path) {
          fprintf(stderr, "Error: command not found: %s\n", cmd[0]);
          free_exported_variable(list_variable_exp);
          _exit(127);
        }

        if (execve(cmd_path, cmd, list_variable_exp) == -1)
        {
            fprintf(stderr, "Error: command not found: %s\n", cmd[0]);
            free_exported_variable(list_variable_exp );
            _exit(127);
        }
        free_exported_variable(list_variable_exp );
    }
    else //parent
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
