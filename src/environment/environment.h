#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "../hash_map/hash_map.h"

struct env
{
	int last_exit_code;
	int argc;

	char **argv;

    struct hash_map *variables;
    // we will have to add another hash_map for functions
};

struct env *init_env(void);

#endif // ! ENVIRONMENT_H
