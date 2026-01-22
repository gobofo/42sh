#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <stdlib.h>
#include <string.h>

#include "../hash_map/hash_map.h"

struct env
{
    // Stores the last exit code of the shell
    int last_exit_code;
	int should_exit;

    int break_count;
	int continue_count;
    // Stores the number of args passed to the shell
    int argc;

    // Stores all args passed to the shell
    char **argv;

    struct hash_map *variables;
	struct hash_map *functions;
};

struct env *init_env(int argc, char **argv);

#endif // ! ENVIRONMENT_H
