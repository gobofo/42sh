
#ifndef MY_42SH_H
#define MY_42SH_H

#include <stdio.h>
#include <time.h>

#include "ast/ast.h"
#include "environment/environment.h"
#include "execution/execution.h"
#include "io_backend/input.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "token.h"


int my_42sh(int argc, char *argv[]);
#endif /* ! MY_42SH_H */
