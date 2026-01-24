#include "utils.h"

static int is_char(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static int is_digit(char c)
{
    return c >= '0' && c <= '9';
}

/**
 * Vérifie si un nom de variable ou fonction est valide selon les règles POSIX.
 * Un nom valide commence par une lettre ou underscore, suivi de lettres, chiffres ou underscores.
 * Retourne 1 si le nom est valide, 0 sinon.
 */

int is_valid_name(char *name)
{
    char first = name[0];

    if (first != '_' && is_char(first) == 0)
        return 0;

    size_t idx = 1;

    while (name[idx] != '\0'
           && (name[idx] == '_' || is_char(name[idx]) || is_digit(name[idx])))
    {
        idx++;
    }

    return name[idx] == '\0';
}
