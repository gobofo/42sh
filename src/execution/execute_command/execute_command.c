#include "execute_command.h"

int execute_non_builtin(char **cmd)
{
    pid_t pid = fork();

    if (pid == -1)
    {
        fprintf(stderr, "Error: fork() failure\n");
        return 1;
    }

    if (pid == 0)
    {
        if (execvp(cmd[0], cmd) == -1)
        {
            fprintf(stderr, "Error: command not found: %s\n", cmd[0]);
            return 127;
        }
    }
    else
    {
        int wstatus;
        int child_pid = waitpid(pid, &wstatus, 0);

        if (child_pid == -1)
        {
            fprintf(stderr, "Error: waitpid() failure\n");
            return 1;
        }

        if (WIFEXITED(wstatus) && WEXITSTATUS(wstatus))
        {
            return 1;
        }
    }

    return 0;
}
