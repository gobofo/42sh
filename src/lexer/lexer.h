#ifndef LEXER_H
#define LEXER_H

#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "lexer_utils.h"
#include "input_stack.h"

#include "../token.h"
#include "../environment/environment.h"
#include "../hash_map/hash_map.h"

struct lexer
{
	int in_var;

	char c;
	char *buffer;

	size_t size;

	FILE *stream;

	struct token *current;
	struct token *next;

	struct input_stack *stack;
};

struct lexer *init_lexer(FILE *input);
void free_lexer(struct lexer *lexer);

struct lexer *get_token(struct lexer *lexer);
void next_token(struct lexer **lexer);

struct token *read_input(struct lexer *lexer);

void free_token(struct token *token);

#endif /* ! LEXER_H */
