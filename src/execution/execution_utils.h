#ifndef EXECUTION_UTILS_H
#define EXECUTION_UTILS_H

#define _POSIX_C_SOURCE 200809L

#include <string.h>
#include <stdio.h>

#include "../ast/ast.h"
#include "../hash_map/hash_map.h"
#include "../environment/environment.h"
#include "../expansion/expansion.h"

int is_valid_name(char *name);
int variable_assignation(struct AST *root);
int create_function(struct AST *root);

char **create_command(struct AST *root);

struct AST **create_redir(struct AST *root);

bool pattern_match(char* word,char *pattern);

char* join_tab_string(char** expanded_v);
#endif /* EXECUTION_UTILS_H */
