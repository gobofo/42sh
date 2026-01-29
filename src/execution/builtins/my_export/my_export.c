#include "my_export.h"

/**
 * Exécute la commande builtin 'export' pour exporter des variables
 * d'environnement. Accepte plusieurs arguments au format VAR=valeur ou VAR pour
 * marquer comme exportée. Retourne 0 en cas de succès ou 2 si aucun argument ou
 * nom de variable invalide.
 */

int my_export(char **command)
{
    if (command[0] == NULL)
    {
        fprintf(stderr, "Error: my_export: no args\n");
        return 2;
    }
    for (int j = 0; command[j] != NULL; j++)
    {
        if (strchr(command[j], '='))
        {
            int i = 0;
            while (command[j][i] != '=')
            {
                i++;
            }
            command[j][i] = 0;
            if (!is_valid_name(command[j]))
            {
                fprintf(stderr, "Error: my_export: not valid Variable Name\n");
                return 2;
            }
            char *value =
                malloc(sizeof(char) * (strlen(&command[j][i + 1]) + 1));
            int pos = i + 1;
            int k = 0;

            while (command[j][pos])
            {
                value[k++] = command[j][pos++];
            }
            value[k] = 0;
            hash_map_insert(env->variables, command[j], value, free);
        }
        export_add_variable(env->export_variables, strdup(command[j]));
    }
    return 0;
}
