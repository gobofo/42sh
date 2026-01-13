#ifndef EXECUTE_COMMAND_H 
#define EXECUTE_COMMAND_H 

#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void execute_command(char *cmd);

#endif /* ! EXECUTE_COMMAND_H  */
