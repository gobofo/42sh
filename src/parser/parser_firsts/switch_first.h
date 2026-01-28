#ifndef SWITCH_FIRST_H
#define SWITCH_FIRST_H

#include <stdbool.h>
#include <stddef.h>

#include "../../token.h"

#include "first_command.h"
#include "first_condition.h"
#include "first_loop.h"
#include "first_token.h"
#include "first_sequence.h"


bool first_rule_case(struct token *token);
bool first_case_item(struct token *token);
bool first_case_clause(struct token *token);

#endif /* ! SWITCH_FIRST_H */




