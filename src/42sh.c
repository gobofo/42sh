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

	struct lexer *lexer = init_lexer(file);
	if (!lexer)
	{
		fprintf(stderr, "Error: Could not create the lexer struct\n");

		fclose(file);

		return 2;
	}

	while (lexer->current != NULL)
	{
		struct AST *ast = input(&lexer);

		if (ast == NULL)
		{
			fprintf(stderr, "Error: Parsing\n");
			
			free_lexer(lexer);

			fclose(file);

			return 2;
		}

		if (pretty_print)
			parser_print(ast);

		env->last_exit_code = execute_ast(ast);
		destroy_AST(ast);
	}

    int return_val = env->last_exit_code;

    hash_map_free(env->variables);

    free(env);
	free_lexer(lexer);

    fclose(file);

    return return_val;
}