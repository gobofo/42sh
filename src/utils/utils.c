#include "utils.h"

/**
 * Vérifie si un nom de variable ou fonction est valide selon les règles POSIX.
 * Un nom valide commence par une lettre ou underscore, suivi de lettres, chiffres ou underscores.
 * Retourne 1 si le nom est valide, 0 sinon.
 */

int is_valid_name(char *name)
{
    char first = name[0];

    if (first != '_' && isalpha(first) == 0)
        return 0;

    size_t idx = 1;

    while (name[idx] != '\0'
           && (name[idx] == '_' || isalpha(name[idx]) || isdigit(name[idx])))
    {
        idx++;
    }

    return name[idx] == '\0';
}
