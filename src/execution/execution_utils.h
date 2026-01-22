#ifndef EXECUTION_UTILS_H
#define EXECUTION_UTILS_H

#define _POSIX_C_SOURCE 200809L

#include <string.h>
#include <stdio.h>

#include "../ast/ast.h"
#include "../hash_map/hash_map.h"
#include "../environment/environment.h"
#include "../expansion/expansion.h"

int variable_assignation(struct AST *root);

char **create_command(struct AST *root);

struct AST **create_redir(struct AST *root);

#endif /* EXECUTION_UTILS_H */
