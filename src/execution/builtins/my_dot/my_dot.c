#include "my_dot.h"

#include <string.h>

extern struct env *env;

/**
 * Exécute la commande builtin '.' (dot) pour sourcer un fichier script.
 * Sauvegarde et remplace temporairement argv/argc avec les arguments fournis.
 * Retourne le code de sortie du script exécuté ou 2 si arguments manquants.
 */

int my_dot(char **command)
{
    if (command[0] == NULL || command[1] == NULL)
    {
        fprintf(stderr, "Error: my_dot: no args\n");
        return 2;
    }

    char **save_argv = env->argv;

    int save_argc = env->argc;

    env->argv = calloc(10, sizeof(char *));
    if (save_argv)
        env->argv[0] = save_argv[0];
    env->argc = 0;

    for (int i = 0; i < 8 && command[i + 2]; i++)
    {
        env->argv[i + 1] = command[i + 2];
        env->argc++;
    }

    int status = my_42sh(2, command);

    free(env->argv);
    env->argv = save_argv;
    env->argc = save_argc;
    return status;
}
