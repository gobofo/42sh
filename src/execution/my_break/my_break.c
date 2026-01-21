#include "my_break.h"

#include <stdio.h>
#include <stdlib.h>
extern struct env *env;

int my_break(char **command)
{
    if (command && command[1] != NULL)
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
