#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <stdlib.h>
#include <string.h>

#include "../hash_map/hash_map.h"

//STRUCT export to manage all the export variables
struct export
{
  char** list_variables;
  int nb_variables;
  int max_variables;
};

// functions of export:
struct export* create_export();

void export_add_variable(struct export *export,char* variables);
void free_export(struct export *export);

struct env
{
    // Stores the last exit code of the shell
    int last_exit_code; // ?
	int should_exit;
	int should_return;

    int break_count;
	int continue_count;
    int boucle_count;
    // Stores the number of args passed to the shell
    int argc; // #

    // Stores all args passed to the shell
    char **argv; // @ *
    
    // Store all exported variables
    struct export *export_variables;

    struct hash_map *variables;
	struct hash_map *functions;
};

struct env *init_env(int argc, char **argv);


#endif // ! ENVIRONMENT_H
