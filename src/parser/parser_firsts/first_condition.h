#ifndef FIRST_CONDITION_H
#define FIRST_CONDITION_H

#include <stdbool.h>
#include <stddef.h>

#include "../../token.h"

#include "first_command.h"
#include "first_loop.h"
#include "first_sequence.h"
#include "first_token.h"

bool first_rule_if(struct token *token);
bool first_else_clause(struct token *token);

#endif /* ! FIRST_CONDITION_H */



