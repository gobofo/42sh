#include "environment.h"

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
    struct env *env = malloc(sizeof(struct env));

    // Creates the hash_map to store the variables
    env->variables = hash_map_init(64);

    // When the shell is launched no command as been launched so the last exit
    // code is 0
    env->last_exit_code = 0;

    // Init the args of the shell
    if (argc > 2 && strcmp(argv[1], "-c") == 0)
    {
        env->argv = &argv[3];
        env->argc = (argc > 3) ? argc - 3 : 0;
    }
    else if (argc > 1)
    {
        env->argv = &argv[2];
        env->argc = argc - 2;
    }
    else
    {
        env->argv = NULL;
        env->argc = 0;
    }

    bool update;

    char *oldpwd = getenv("OLDPWD");
    char *pwd = getenv("PWD");
    char *ifs = getenv("IFS");
    if (!ifs)
        ifs = "\t\n";

	// ENV Variables Predefined when the shell is launched
	if(oldpwd)
		hash_map_insert(env->variables, "OLDPWD", oldpwd, &update);
	hash_map_insert(env->variables, "PWD", pwd ? pwd : ".", &update);
	hash_map_insert(env->variables, "IFS", ifs, &update);

	return env;
}
