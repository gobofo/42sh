#ifndef INPUT_STACK_H
#define INPUT_STACK_H

struct input_stack
{
	FILE *file;

	char *alias_name;

	struct input_stack *next;
};

struct input_stack *init_input_stack(FILE *input);

#endif /* ! INPUT_STACK_H */
