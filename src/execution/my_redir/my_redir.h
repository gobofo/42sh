#ifndef MY_REDIR_H
#define MY_REDIR_H


int redir_replace_in(char **command, struct AST **redir, int fd);
int redir_append_in(char **command, struct AST **redir, int fd);
int redir_read(char **command, struct AST **redir, int fd);
int redir_dup(char **command, struct AST **redir, int fd);
int redir_open(char **command, struct AST **redir, int fd);

#endif /* ! MY_REDIR_H */
