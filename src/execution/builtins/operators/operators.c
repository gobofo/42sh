#include "operators.h"

int my_true(char **command)
{
    (void)command;
    return 0;
}

int my_false(char **command)
{
    (void)command;
    return 1;
}

int my_return(char **command)
{
    env->should_return = 1;
	
	if (command[0])
		return atoi(command[0]);

	return 0;
}
