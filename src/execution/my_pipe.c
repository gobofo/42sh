#include "my_pipe.h"

#include <err.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int exec_pipe(char **argv_left, char **argv_right)
{
    int pipe_fd[2];

    if (pipe(pipe_fd) == -1)
        errx(1, "pipe() Failure");

    pid_t pid_left = fork();

    if (pid_left == -1)
        errx(1, "fork() Failure");

    if (pid_left == 0)
    {
        if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
            errx(1, "dup2() Failure");

        close(pipe_fd[0]);
        close(pipe_fd[1]);

        execvp(argv_left[0], argv_left);
        exit(1);
    }

    pid_t pid_right = fork();

    if (pid_right == -1)
        errx(1, "fork() Failure");

    if (pid_right == 0)
    {
        if (dup2(pipe_fd[0], STDIN_FILENO) == -1)
            errx(1, "dup2() Failure");

        close(pipe_fd[0]);
        close(pipe_fd[1]);

        execvp(argv_right[0], argv_right);
        exit(1);
    }
    else
    {
        close(pipe_fd[0]);
        close(pipe_fd[1]);

        int wstatus;
        int child_pid_left = waitpid(pid_left, NULL, 0);
        int child_pid_right = waitpid(pid_right, &wstatus, 0);

        if (child_pid_left == -1 || child_pid_right == -1)
            errx(1, "waitpid Failure for right child");

        if (WIFEXITED(wstatus))
        {
            int status = WEXITSTATUS(wstatus);

            return status;
        }
    }

    return 1;
}
