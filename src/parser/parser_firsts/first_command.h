#ifndef FIRST_COMMAND_H
#define FIRST_COMMAND_H

#include <stdbool.h>
#include <stddef.h>

#include "../../token.h"

#include "first_condition.h"
#include "first_loop.h"
#include "first_sequence.h"
#include "first_token.h"
#include "switch_first.h"

bool first_command(struct token *token);
bool first_shell_command(struct token *token);
bool first_simple_command(struct token *token);
bool first_funcdec(struct token *token);

#endif /* ! FIRST_COMMAND_H */





