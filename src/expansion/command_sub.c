#define _POSIX_C_SOURCE 200809L
#include "command_sub.h"

#include "../42sh.h"

extern struct env *env;

char *expand_command_substitution(char *sub_string)
{
    int pipefd[2];
    if (pipe(pipefd) == -1)
    {
        fprintf(stderr, "%s\n", "Error: pipefd");
        return NULL;
    }
    pid_t pid = fork();
    if (pid == 0)
    {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        char *argv[] = { "./src/42sh", "-c", sub_string, NULL };
        _exit(my_42sh(3, argv));
    }
    else
    {
        close(pipefd[1]);
        char *output = NULL;
        size_t output_len = 0;

        FILE *output_file = open_memstream(&output, &output_len);
        char buf[2];
        while (read(pipefd[0], buf, 1) > 0)
        {
            fputc(buf[0], output_file);
        }
        fclose(output_file);
        close(pipefd[0]);

        int wstatus;
        waitpid(pid, &wstatus, 0);
        env->last_exit_code=WEXITSTATUS(wstatus);

        if (output_len > 0 && output[output_len - 1] == '\n')
        {
            output[output_len - 1] = '\0';
        }

        return output;
    }
}
