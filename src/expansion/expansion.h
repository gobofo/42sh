#ifndef EXPANSION_H
#define EXPANSION_H

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../environment/environment.h"

char *expand(char **value);

#endif /* ! EXPANSION_H */
