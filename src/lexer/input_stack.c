#include "input_stack.h"

struct input_stack *init_input_stack(FILE *input)
{
	struct input_stack *stack = malloc(sizeof(struct input_stack));
	if (stack == NULL)
		return NULL;

	stack->file = input;
	stack->alias_name = NULL;
	stack->next = NULL;

	return stack;
}

void free_input_stack(struct input_stack *stack)
{
	struct input_stack *cur = stack;
	while (cur)
	{
		struct input_stack *temp = cur;
		cur = cur->next;

		if (temp->alias_name)
		{
			fclose(temp->file);
			free(temp->alias_name);
		}

		free(temp);
	}
}
