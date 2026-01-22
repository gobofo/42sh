#ifndef MY_CD_H
#define MY_CD_H

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../../environment/environment.h"
#include "../../../hash_map/hash_map.h"

int my_cd(char **command);

#endif /* ! MY_CD_H */
