#ifndef FIRST_SEQUENCE_H
#define FIRST_SEQUENCE_H

#include <stdbool.h>
#include <stddef.h>

#include "../../token.h"

#include "first_command.h"
#include "first_condition.h"
#include "first_loop.h"
#include "first_token.h"

bool first_list(struct token *token);
bool first_and_or(struct token *token);
bool first_pipeline(struct token *token);
bool first_compound_list(struct token *token);

#endif /* ! FIRST_SEQUENCE_H */



