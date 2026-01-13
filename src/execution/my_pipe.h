#ifndef MYPIPE_H
#define MYPIPE_H

#include <err.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int exec_pipe(char **argv_left, char **argv_right);

#endif /* ! MYPIPE_H */
