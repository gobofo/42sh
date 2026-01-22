#ifndef MY_EXPORT_H
#define MY_EXPORT_H

#define _POSIX_C_SOURCE  200809L

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../../utils.h"
#include "../../../environment/environment.h"

int my_export(char **command);

#endif /* ! MY_EXPORT_H */
