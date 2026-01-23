#ifndef FOLLOW_COMMAND_H
#define FOLLOW_COMMAND_H

#include <stdbool.h>
#include <stddef.h>

#include "../../token.h"

#include "follow_token.h"
#include "follow_condition.h"
#include "follow_loop.h"
#include "follow_sequence.h"

bool follow_command(struct token *token);
bool follow_shell_command(struct token *token);
bool follow_simple_command(struct token *token);
bool follow_funcdec(struct token *token);

#endif /* ! FOLLOW_COMMAND_H */




