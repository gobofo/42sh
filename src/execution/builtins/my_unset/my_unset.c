#include "my_unset.h"

/**
 * Exécute la commande builtin 'unset' pour supprimer des variables ou
 * fonctions. Accepte les flags -v (variables, par défaut) et -f (fonctions)
 * suivis des noms. Retourne 0 en cas de succès, 1 si les deux flags sont
 * utilisés, 2 si flag invalide.
 */

int my_unset(char **command)
{
    int v_flag = 0;
    int f_flag = 0;

    if (command[0][0] == '-')
    {
        for (int i = 1; command[0][i]; i++)
        {
            if (command[0][i] == 'v')
                v_flag = 1;
            else if (command[0][i] == 'f')
                f_flag = 1;
            else
            {
                fprintf(stderr, "Error: unset: flag must be v or f\n");
                return 2;
            }
        }
        command++;
    }

    if (f_flag && v_flag)
    {
        fprintf(stderr, "Error: unset: flag must be v or f not both\n");
        return 1;
    }

    if (!(f_flag || v_flag) || v_flag)
        for (int i = 0; command[i]; i++)
        {
            bool removed = hash_map_remove(env->variables, command[i], free);
            if (!removed && !v_flag)
                hash_map_remove(env->functions, command[i], destroy_AST_void);
        }

    else if (f_flag)
        for (int i = 0; command[i]; i++)
            hash_map_remove(env->functions, command[i], destroy_AST_void);

    return 0;
}
