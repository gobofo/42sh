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

void push_input_stack(struct input_stack **stack, FILE *input, char *alias_name)
{
    struct input_stack *new_input = malloc(sizeof(struct input_stack));
    if (new_input == NULL)
        return;

    new_input->file = input;
    new_input->alias_name = alias_name ? strdup(alias_name) : NULL;
    new_input->next = *stack;

    *stack = new_input;
}

void pop_input_stack(struct input_stack **stack)
{
    if (stack == NULL || *stack == NULL)
        return;

    struct input_stack *top = *stack;
    *stack = top->next;

    if (top->alias_name)
    {
        fclose(top->file);
        free(top->alias_name);
    }

    free(top);
}

void free_input_stack(struct input_stack *stack)
{
    while (stack != NULL)
        pop_input_stack(&stack);
}
