#ifndef EXECUTION_H
#define EXECUTION_H

#define _POSIX_C_SOURCE 200809L

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../ast/ast.h"
#include "../environment/environment.h"
#include "../expansion/expansion.h"
#include "../hash_map/hash_map.h"
#include "../parser/parser.h"
#include "execute_command/execute_command.h"
#include "my_cd/my_cd.h"
#include "my_echo/my_echo.h"
#include "my_exit/my_exit.h"
#include "my_redir/my_redir.h"
#include "my_break/my_break.h"
#include "my_continue/my_continue.h"
#include "my_export/my_export.h"


int execute_ast(struct AST *root);

#endif /* ! EXECUTION_H */
