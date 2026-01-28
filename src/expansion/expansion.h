#ifndef EXPANSION_H
#define EXPANSION_H

#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "command_sub.h"
#include "expansion_utils.h"

#include "../environment/environment.h"

extern struct env *env;

struct expanded_words
{
	char **words;
	size_t count;
};

struct expansion_context
{
	FILE *stream;

	int quoted;
	int is_assign;

	char **buffer;

	size_t *size;

	struct expanded_words *words;
};

char **expand(char *str, int is_assign);

#endif /* ! EXPANSION_H */
