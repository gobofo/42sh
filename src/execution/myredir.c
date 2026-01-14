#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
int read_file(char *file_path)
{
    int fd = open(file_path, O_CREAT | O_RDWR | O_TRUNC, 0644);
    if (fd == -1) // Could not open the file
        return -1;

    return fd;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
        errx(2, "Usage ./myredir <file_name> <command_to_execute>");

    int fd = read_file(argv[1]);
    if (fd == -1)
        errx(1, "Could not open the file: %s", argv[1]);

    pid_t pid = fork();

    if (pid == -1)
        errx(1, "Fork Failure");

    if (pid == 0)
    {
        if (dup2(fd, STDOUT_FILENO) == -1)
            errx(1, "dup2 Failure");

        close(fd);

        execvp(argv[2], &argv[2]);
        warn("%s", argv[2]);

        exit(127);
    }
    else
    {
        close(fd);

        int wstatus;
        int child_pid = waitpid(pid, &wstatus, 0);

        if (child_pid == -1)
            errx(1, "Failure while waiting for child pid");

        if (WIFEXITED(wstatus))
        {
            int status = WEXITSTATUS(wstatus);

            if (status == 127)
                return 1;

            printf("%s exited with %d!\n", argv[2], status);
        }
    }
    return 0;
}

>
>>

<
>|
>&
<&
<>
