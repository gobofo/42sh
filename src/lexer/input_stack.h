#ifndef INPUT_STACK_H
#define INPUT_STACK_H

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


struct input_stack
{
	FILE *file;

	char *alias_name;

	struct input_stack *next;
};

struct input_stack *init_input_stack(FILE *input);

void push_input_stack(struct input_stack **stack,
		FILE *input, char *alias_name);

void pop_input_stack(struct input_stack **stack);
void free_input_stack(struct input_stack *stack);

#endif /* ! INPUT_STACK_H */
