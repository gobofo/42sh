#define _POSIX_C_SOURCE 200809L

#include "input.h"

FILE *get_input_file(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-c") == 0)
        {
            if (i + 1 < argc)
                return fmemopen(argv[i + 1], strlen(argv[i + 1]), "r");
            else
                return NULL;
        }
    }

    if (argc > 1)
    {
        FILE *file = fopen(argv[1], "r");
        if (!file)
            return NULL;

        return file;
    }

    return stdin;
}
