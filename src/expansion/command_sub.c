#include "command_sub.h"

extern struct env *env;

/**
 * Exécute une substitution de commande et capture sa sortie standard.
 * Crée un pipe, fork un processus pour exécuter la commande, lit la sortie.
 * Retourne la sortie de la commande (sans le dernier '\n') ou NULL en cas
 * d'erreur.
 */

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
        env->last_exit_code = WEXITSTATUS(wstatus);

        if (output_len > 0 && output[output_len - 1] == '\n')
        {
            output[output_len - 1] = '\0';
        }

        return output;
    }
}
