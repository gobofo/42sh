#ifndef FOLLOW_SEQUENCE_H
#define FOLLOW_SEQUENCE_H

#include <stdbool.h>
#include <stddef.h>

#include "../../token.h"

#include "follow_token.h"
#include "follow_command.h"
#include "follow_condition.h"
#include "follow_loop.h"

bool follow_list(struct token *token);
bool follow_and_or(struct token *token);
bool follow_pipeline(struct token *token);
bool follow_compound_list(struct token *token);


#endif /* ! FOLLOW_SEQUENCE_H */




