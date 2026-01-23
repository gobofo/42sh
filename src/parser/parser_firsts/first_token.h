#ifndef FIRST_TOKEN_H
#define FIRST_TOKEN_H

#include <stdbool.h>
#include <stddef.h>

#include "../../token.h"

#include "first_command.h"
#include "first_condition.h"
#include "first_loop.h"
#include "first_sequence.h"

bool first_prefix(struct token *token);
bool first_redirection(struct token *token);
bool first_element(struct token *token);

#endif /* ! FIRST_TOKEN_H */




