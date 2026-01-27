#define _POSIX_C_SOURCE 200809L
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

    env->argv = calloc(argc + 1, sizeof(char *));
    env->argv[0] = argv[0];
    env->argc = 0;

    int i = 1;
    if (i < argc)
    {
        // Case we use a -c to launch the program
        if (strcmp(argv[i], "-c") == 0)
        {
            // Skip the -c
            i += 2;
			env->argc--;
        }
        // Case we give a script
        else
        {
            env->argv[0] = argv[i];
            i++;
        }

        // Get all the arguments
        while (i < argc)
        {
            env->argc++;
            env->argv[env->argc] = argv[i];
            i++;
        }
		if(env->argc<0)
			env->argc = 0;
    }

    char *oldpwd = getenv("OLDPWD");
    char *pwd = getenv("PWD");
    char *ifs = getenv("IFS");
	char *home = getenv("HOME");

    if (!ifs)
        ifs = " \t\n";

    // ENV Variables Predefined when the shell is launched
    if (oldpwd)
        hash_map_insert(env->variables, "OLDPWD", strdup(oldpwd), free);

    hash_map_insert(env->variables, "PWD", strdup(pwd ? pwd : "."), free);
    hash_map_insert(env->variables, "IFS", strdup(ifs), free);

	if (home)
        hash_map_insert(env->variables, "HOME", strdup(home), free);

    struct export *export_variables = create_export();
    env->export_variables = export_variables;

    return env;
}

/**
 * Libère la mémoire allouée pour la structure env.
 * Free d'abord le tableau argv puis la structure elle-même.
 */
void free_env(struct env *env)
{
    free(env->argv);
    free(env);
}

/**
 * Crée et initialise une nouvelle structure export pour stocker des variables.
 * Alloue un tableau initial de 8 emplacements pour les variables.
 * Retourne un pointeur vers la structure créée.
 */
struct export *create_export(void)
{
    struct export *export = malloc(sizeof(struct export));

    export->nb_variables = 0;
    export->max_variables = 8;
    export->list_variables = malloc(export->max_variables * sizeof(char *));

    return export;
}

/**
 * Ajoute une variable à la liste d'export en gérant l'allocation dynamique.
 * Double la capacité du tableau si nécessaire (croissance exponentielle).
 * Incrémente le compteur de variables après l'ajout.
 */
void export_add_variable(struct export *export, char *variables)
{
    if (export->nb_variables >= export->max_variables)
    {
        export->max_variables = export->max_variables * 2;
        export->list_variables = realloc(
            export->list_variables, export->max_variables * sizeof(char *));
    }

    export->list_variables[export->nb_variables] = variables;
    export->nb_variables++;
}

/**
 * Libère la mémoire de la structure export et de toutes ses variables.
 * Free chaque chaîne de caractère dans la liste, puis la liste, puis la
 * structure.
 */

void free_export(struct export *export)
{
    for (int i = 0; i < export->nb_variables; i++)
        free(export->list_variables[i]);

    free(export->list_variables);
    free(export);
}
