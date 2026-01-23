#ifndef FOLLOW_CONDITION_H
#define FOLLOW_CONDITION_H

#include <stdbool.h>
#include <stddef.h>

#include "../../token.h"

#include "follow_token.h"
#include "follow_command.h"
#include "follow_loop.h"
#include "follow_sequence.h"

bool follow_rule_if(struct token *token);
bool follow_else_clause(struct token *token);

#endif /* ! FOLLOW_CONDITION_H */




