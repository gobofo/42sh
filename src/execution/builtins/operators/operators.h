#ifndef OPERATORS_H
#define OPERATORS_H

#include <stdio.h>
#include <stdlib.h>

#include "../../../environment/environment.h"

extern struct env *env;

int my_true(char **command);
int my_false(char **command);
int my_return(char **command);

#endif /* ! OPERATORS_H */
