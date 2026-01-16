#include "environment.h"

#include <stdlib.h>

struct env *init_env(void)
{
    struct env *env = malloc(sizeof(struct env));
    env->variables = hash_map_init(64);
	env->last_exit_code = 0;

    bool update;

	char *oldpwd = getenv("OLDPWD");
	char *pwd = getenv("PWD");
	char *ifs = getenv("IFS");
	if (!ifs)
		ifs = "\t\n";

	// ENV Variables Predefined when the shell is launched
    hash_map_insert(env->variables, "OLDPWD", oldpwd, &update);
    hash_map_insert(env->variables, "PWD", pwd ? pwd : ".", &update);
    hash_map_insert(env->variables, "IFS", ifs, &update);

    return env;
}
