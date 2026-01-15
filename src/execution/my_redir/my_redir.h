#ifndef MY_REDIR_H
#define MY_REDIR_H

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../../environment/environment.h"
#include "../../ast/ast.h"

int redir_replace_in(struct AST *root, struct AST **redir, struct env *env,
		int fd);

int redir_append_in(struct AST *root, struct AST **redir, struct env *env,
		int fd);

int redir_read(struct AST *root, struct AST **redir, struct env *env,
		int fd);

int redir_dup(struct AST *root, struct AST **redir, struct env *env,
		int fd);

int redir_open(struct AST *root, struct AST **redir, struct env *env,
		int fd);

#endif /* ! MY_REDIR_H */
