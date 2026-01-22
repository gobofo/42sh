#ifndef EXECUTION_UTILS_H
#define EXECUTION_UTILS_H


#include <stdio.h>

#include "../ast/ast.h"
#include "../hash_map/hash_map.h"
#include "../environment/environment.h"
#include "../expansion/expansion.h"

int variable_assignation(struct AST *root);

#endif /* EXECUTION_UTILS_H */
