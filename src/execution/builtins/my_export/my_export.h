#ifndef MY_EXPORT_H
#define MY_EXPORT_H

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../../environment/environment.h"
#include "../../../lexer/lexer_utils.h"

extern struct env *env;

int my_export(char **command);

#endif /* ! MY_EXPORT_H */
