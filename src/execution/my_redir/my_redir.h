#ifndef MY_REDIR_H
#define MY_REDIR_H

#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include "../../ast/ast.h"

int redir_replace_in(struct AST* root, struct AST **redir, int fd);
int redir_append_in(struct AST* root, struct AST **redir, int fd);
int redir_read(struct AST* root, struct AST **redir, int fd);
int redir_dup(struct AST* root, struct AST **redir, int fd);
int redir_open(struct AST* root, struct AST **redir, int fd);

#endif /* ! MY_REDIR_H */
