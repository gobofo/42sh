#include "launch_shell.h"

struct env *env; // structure qui contient l'environement et toutes les hash map

int main(int argc, char *argv[])
{
    // Ensure random numbers for $RANDOM for two shells launched at the same
    // time
    srand(time(NULL) ^ getpid());
    env = init_env(argc, argv);
    int exit_code = my_42sh(argc, argv);

    // free env
    hash_map_free(env->variables, free);
    hash_map_free(env->functions, destroy_AST_void);
    hash_map_free(env->alias, free);
    free_export(env->export_variables);
    free_env(env);

    return exit_code;
}
