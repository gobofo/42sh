#ifndef MY_CD_H
#define MY_CD_H

#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../../hash_map/hash_map.h"
#include "../../environment/environment.h"

int my_cd(char **command);

#endif /* ! MY_CD_H */
