#ifndef EXPANSION_H
#define EXPANSION_H

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../environment/environment.h"

struct expanded_words
{
	char **words;
	size_t count;
};

struct expansion_context
{
	FILE *stream;

	int quoted;

	char **buffer;

	size_t *size;

	struct expanded_words *words;
};

char **expand(char *str);

#endif /* ! EXPANSION_H */
