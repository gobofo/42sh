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

#include "../token.h"
#include "../environment/environment.h"

struct input_stack
{
	FILE *file;

	char *alias_name;

	struct input_stack *next;
};

struct lexer
{
	FILE *input;

	struct token *current;
	struct token *next;

	struct input_stack *stack;
};

struct lexer *init_lexer(FILE *input);
void free_lexer(struct lexer *lexer);

struct lexer *get_token(struct lexer *lexer);
void next_token(struct lexer **lexer);

struct token *read_input(FILE *file);

void free_token(struct token *token);

#endif /* ! LEXER_H */
