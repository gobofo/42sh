#include "my_continue.h"

#include <stdio.h>
#include <stdlib.h>
extern struct env *env;

/**
 * Exécute la commande builtin 'continue' pour passer à l'itération suivante de boucles.
 * Accepte un argument optionnel n pour continuer n niveaux de boucles imbriquées.
 * Retourne 0 en cas de succès ou 2 si l'argument est invalide ou trop d'arguments.
 */

int my_continue(char **command)
{
    if (command[0] && command[1] != NULL)
    {
        fprintf(stderr, "Error: continue: too many arguments");
        return 2;
    }
    env->continue_count = 1;
    if (command[0])
    { // break X
        int nb = atoi(command[0]);
        if (nb == 0)
        {
            fprintf(stderr, "Error: continue: not valid argument");
            return 2;
        }
        env->continue_count = nb;
    }
    return env->last_exit_code;
}
