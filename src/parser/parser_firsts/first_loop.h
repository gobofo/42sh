#ifndef FIRST_LOOP_H
#define FIRST_LOOP_H

#include <stdbool.h>
#include <stddef.h>

#include "../../token.h"

#include "first_command.h"
#include "first_condition.h"
#include "first_sequence.h"
#include "first_token.h"

bool first_rule_while(struct token *token);
bool first_rule_until(struct token *token);
bool first_rule_for(struct token *token);

#endif /* ! FIRST_LOOP_H */



