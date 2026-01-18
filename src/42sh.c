#include <stdio.h>
#include <time.h>

#include "ast/ast.h"
#include "environment/environment.h"
#include "execution/execution.h"
#include "io_backend/input.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "token.h"

struct env *env;

int main(int argc, char *argv[])
{
    // Ensure random numbers for $RANDOM for two shells launched at the same
    // time
    srand(time(NULL) ^ getpid());

    // PRETTY PRINT A ACTIVER AVEC PRETTY_PRINT=1 dans le terminal
    int pretty_print = 0;

    char *pretty_print_value = getenv("PRETTY_PRINT");

    if (pretty_print_value)
        pretty_print = (atoi(pretty_print_value) == 1);

    FILE *file = get_input_file(argc, argv);

    if (file == NULL)
    {
        fprintf(stderr, "Error: IO\n");
        return 2;
    }

    env = init_env(argc, argv);

    struct token *tok = get_token(file);
    while (1)
    {
        struct AST *ast;

        if (tok == NULL)
            break;

        else
        {
            ast = input(&tok);

            if (ast == NULL)
            {
                fprintf(stderr, "Error: Parsing\n");
                return 2;
            }
        }

        if (pretty_print)
            parser_print(ast);

        env->last_exit_code = execute_ast(ast);
        destroy_AST(ast);

        if (tok == NULL)
            break;

        tok = get_token(NULL);
    }

    int return_val = env->last_exit_code;
    hash_map_free(env->variables);

    for (int i = 0; i < env->argc; i++)
        free(env->argv[i]);
    free(env);

    fclose(file);

    return return_val;
}
