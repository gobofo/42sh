#define _POSIX_C_SOURCE 200809L

#include "input.h"

#include <string.h>

FILE *getInputFile(int argc, char *argv[])
{
    if (argc == 1)
        return stdin;

    if (argc == 2)
        return fopen(argv[1], "r");

    if (argc != 3)
        return NULL;

    if (strcmp(argv[1], "-c") == 0)
        return fmemopen(argv[2], strlen(argv[2]), "r");

    return NULL;
}
