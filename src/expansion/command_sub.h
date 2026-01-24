#ifndef COMMAND_SUB_H
#define COMMAND_SUB_H

#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "../launch_shell.h"
#include "../environment/environment.h"

char* expand_command_substitution(char* sub_string);

#endif /* ! COMMAND_SUB_H */
