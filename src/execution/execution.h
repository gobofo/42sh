#ifndef EXECUTION_H
#define EXECUTION_H

#define _POSIX_C_SOURCE 200809L

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "execute_command/execute_command.h"
#include "my_pipe.h"
#include "my_echo/my_echo.h"
#include "my_redir/my_redir.h"

#include "../ast/ast.h"
#include "../parser/parser.h"
#include "../expansion/expansion.h"

int execute_ast(struct AST *root);


#endif /* ! EXECUTION_H */
