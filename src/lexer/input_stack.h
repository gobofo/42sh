#ifndef INPUT_STACK_H
#define INPUT_STACK_H

#include <stdio.h>
#include <stdlib.h>

struct input_stack
{
	FILE *file;

	char *alias_name;

	struct input_stack *next;
};

struct input_stack *init_input_stack(FILE *input);
void free_input_stack(struct input_stack *stack);

#endif /* ! INPUT_STACK_H */
