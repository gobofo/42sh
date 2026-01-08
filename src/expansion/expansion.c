#define _POSIX_C_SOURCE 200809L

#include "expansion.h"

int expand(char **value)
{
    char *p = *value;
    char *buff;

    size_t size;

    FILE *stream = open_memstream(&buff, &size);

    while (*p)
    {
        if (*p != '\'')
            fputc(*p, stream);

        if (*p == 0)
            goto error;

        p++;
    }

    fclose(stream);
    free(*value);

    *value = buff;

    return 0;

error:
    fclose(stream);
    free(buff);

    return 1;
}
