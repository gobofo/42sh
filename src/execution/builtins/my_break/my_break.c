#include "my_break.h"

/**
 * Exécute la commande builtin 'break' pour sortir de boucles imbriquées.
 * Accepte un argument optionnel n pour sortir de n niveaux de boucles.
 * Retourne 0 en cas de succès ou 2 si l'argument est invalide ou trop
 * d'arguments.
 */

int my_break(char **command)
{
    if (command[0] && command[1] != NULL)
    {
        fprintf(stderr, "Error: break: too many arguments");
        return 2;
    }
    env->break_count = 1;
    if (command[0])
    { // break X
        int nb = atoi(command[0]);
        if (nb == 0)
        {
            fprintf(stderr, "Error: break: not valid argument");
            return 2;
        }
        env->break_count = nb;
    }
    return env->last_exit_code;
}
