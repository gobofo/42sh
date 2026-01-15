#include "environment.h"
#include <stdlib.h>

struct env *init_env(void){
	struct env* env = malloc(sizeof(struct env));
	env->variables = hash_map_init(64);

	bool update;
	hash_map_insert(env->variables, "OLDPWD", getenv("OLDPWD"), &update);	
	hash_map_insert(env->variables, "PWD", getenv("PWD"), &update);
	hash_map_insert(env->variables, "IFS", getenv("IFS"), &update);

	return env;
}
