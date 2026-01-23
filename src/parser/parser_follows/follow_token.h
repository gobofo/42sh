#ifndef FOLLOW_TOKEN_H
#define FOLLOW_TOKEN_H

#include <stdbool.h>
#include <stddef.h>

#include "../../token.h"

#include "follow_command.h"
#include "follow_condition.h"
#include "follow_loop.h"
#include "follow_sequence.h"
#include "../parser_firsts/first_token.h"

bool follow_prefix(struct token *token);
bool follow_redirection(struct token *token);
bool follow_element(struct token *token);

#endif /* ! FOLLOW_TOKEN_H */




