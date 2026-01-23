#ifndef EXPANSION_UTILS_H
#define EXPANSION_UTILS_H

#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int is_special_char(char c);
int is_valid_identifier(char *name);

char **separate_white_space(char *output);

#endif /* ! EXPANSION_UTILS_H */
