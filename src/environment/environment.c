#define _POSIX_C_SOURCE 200809L
#include "environment.h"
#include <stdio.h>

/**
 * @brief		Inits the environment
 *
 * When the shell is launched, we init an empty environment to store all the
 * date inside
 *
 * @param argc	The number of args passed to the shell
 * @param argv	The different args passed to the shell
 *
 * @return		The new environment
 */

struct env *init_env(int argc, char **argv)
{
    struct env *env = calloc(1, sizeof(struct env));

    // Creates the hash_map to store the variables
    env->variables = hash_map_init(64);
	env->functions = hash_map_init(64);

    // When the shell is launched no command as been launched so the last exit
    // code is 0
    env->last_exit_code = 0;

	// Keep track if the use calls the exit command
	env->should_exit = 0;

	env->should_return = 0;

    // Init the args of the shell
    if (argc > 2 && strcmp(argv[1], "-c") == 0)
    {
        env->argv = argv + 2;
        env->argc = (argc > 3) ? argc - 3 : 0;
    }
    else if (argc > 1)
    {
        env->argv = argv + 1;
        env->argc = argc - 2;
    }
    else
    {
        env->argv = NULL;
        env->argc = 0;
    }



    char *oldpwd = getenv("OLDPWD");
    char *pwd = getenv("PWD");
    char *ifs = getenv("IFS");

    if (!ifs)
        ifs = "\t\n";

    // ENV Variables Predefined when the shell is launched
    if (oldpwd)
        hash_map_insert(env->variables, "OLDPWD", strdup(oldpwd), free);

    hash_map_insert(env->variables, "PWD", strdup(pwd ? pwd : "."), free);
    hash_map_insert(env->variables, "IFS", strdup(ifs), free);

    struct export* export_variables= create_export();
    env->export_variables=export_variables;

    return env;
}

struct export* create_export(void){
  struct export* export=malloc(sizeof(struct export));
  export->nb_variables=0;
  export->max_variables=8;
  export->list_variables=malloc(export->max_variables*sizeof(char*));
  return export;
}

void export_add_variable(struct export *export,char* variables){
  if(export->nb_variables>=export->max_variables){
    export->max_variables=export->max_variables*2;
    export->list_variables=realloc(export->list_variables, export->max_variables*sizeof(char*));
  }
  export->list_variables[export->nb_variables]=variables;
  export->nb_variables++;
}

void free_export(struct export *export){
  for(int i=0;i<export->nb_variables;i++){
    free(export->list_variables[i]);
  }
  free(export->list_variables);
  free(export);
}
