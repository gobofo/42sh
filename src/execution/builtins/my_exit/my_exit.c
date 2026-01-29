#include "my_exit.h"

int my_atoi(char *str, int *is_valid)
{
    int res = 0;
    int start_idx = 0;
    int is_neg = 0;

    if (str[0] == '-')
    {
        is_neg = 1;
        start_idx = 1;
    }

    for (int i = start_idx; str[i] != '\0'; i++)
    {
        if (str[i] < '0' || str[i] > '9')
        {
            *is_valid = 0;
            return -1;
        }

        int digit = str[i] - '0';
        res = res * 10 + digit;
    }

    if (is_neg == 1)
        res = -1 * res;

    return res;
}

/**
 * Exécute la commande builtin 'exit' pour terminer le shell.
 * Accepte un argument optionnel pour spécifier le code de sortie (0-255).
 * Retourne le code de sortie spécifié ou celui de la dernière commande si non
 * précisé.
 */

int my_exit(char **command)
{
    // Exit takes 1 or no arguments
    if (command[0] && command[1] != NULL)

    {
        fprintf(stderr, "Error: exit: too many arguments\n");
        return 2;
    }

    // If no exit code is precised we return the one of the last command
    // executed
    if (command[0] == NULL)
    {
        env->should_exit = 1;
        return env->last_exit_code;
    }

    // We need to ensure that the arguments passed is an int
    int is_valid = 1;
    int exit_code = my_atoi(command[0], &is_valid);

    // The argument is not a number
    if (is_valid == 0)
    {
        fprintf(stderr, "Error: exit: %s: numeric argument required\n",
                command[0]);
        return 2;
    }

    // The only exit codes valid are between 0 and 255 so we apply a mod on it
    exit_code = exit_code % 256;

    env->should_exit = 1;

    return exit_code;
}
