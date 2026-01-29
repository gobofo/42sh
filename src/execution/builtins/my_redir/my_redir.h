#ifndef MY_REDIR_H
#define MY_REDIR_H

#define _POSIX_C_SOURCE 200809L

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../../../ast/ast.h"
#include "../../../environment/environment.h"
#include "../../../expansion/expansion.h"

struct redir
{
    char *type;

    int flags;
    int fd;
};

int execute_redir(struct AST *root, struct AST **redir);

#endif /* ! MY_REDIR_H */
