#ifndef FOLLOW_LOOP_H
#define FOLLOW_LOOP_H

#include <stdbool.h>
#include <stddef.h>

#include "../../token.h"

#include "follow_token.h"
#include "follow_command.h"
#include "follow_condition.h"
#include "follow_sequence.h"
#include "switch_follow.h"

bool follow_rule_while(struct token *token);
bool follow_rule_until(struct token *token);
bool follow_rule_for(struct token *token);

#endif /* ! FOLLOW_LOOP_H */




