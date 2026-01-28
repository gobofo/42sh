
#ifndef SWITCH_FOLLOW_H
#define SWITCH_FOLLOW_H

#include <stdbool.h>
#include <stddef.h>

#include "../../token.h"

#include "follow_command.h"
#include "follow_condition.h"
#include "follow_loop.h"
#include "follow_sequence.h"
#include "follow_token.h"
#include "../parser_firsts/first_token.h"


bool follow_rule_case(struct token *token);
bool follow_case_clause(struct token *token);
bool follow_case_item(struct token *token);

#endif /* ! SWITCH_FOLLOW_H */




