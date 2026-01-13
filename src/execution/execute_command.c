#include "execute_command.h"

void execute_command(char *cmd)
{
    pid_t pid = fork();

    if (pid == -1)
		printf("fork() failure");

    if (pid == 0)
    {
        if (execlp("/bin/sh", "supershell", "-c", cmd, NULL) == -1)
			printf("%s command not found", cmd);
    }
    else
    {
        int wstatus;
        int child_pid = waitpid(pid, &wstatus, 0);

        if (child_pid == -1)
			printf("waitpid() failure");

        if (WIFEXITED(wstatus) && WEXITSTATUS(wstatus))
			printf("%s command failure", cmd);
    }
}
