
#include "42sh.h"

struct env *env; //structure qui contient l'environement et toutes les hash map

int my_42sh(int argc, char *argv[])
{
    // PRETTY PRINT A ACTIVER AVEC PRETTY_PRINT=1 dans le terminal
    int pretty_print = 0;

    char *pretty_print_value = getenv("PRETTY_PRINT");

    if (pretty_print_value)
        pretty_print = (atoi(pretty_print_value) == 1);

    FILE *file = get_input_file(argc, argv); //IO_backend pour transformer ce qui est donner en arg en FILE

    if (file == NULL) //erreur IO
    {
        fprintf(stderr, "Error: IO\n");
        return 2;
    }


	struct lexer *lexer = init_lexer(file);//creer le lexer
	if (!lexer)//erreur de lexing
	{
		fprintf(stderr, "Error: Could not create the lexer struct\n");

		fclose(file);

		return 2;
	}

	while (lexer->current != NULL) //pour traiter tous les AST
	{
		struct AST *ast = input(&lexer); //fonction principale du parser (renvoi l'ast)

		if (ast == NULL) //erreur de Parsing
		{
			fprintf(stderr, "Error: Parsing\n");
			
			free_lexer(lexer);

			fclose(file);

			return 2;
		}

		if (pretty_print) //si pretty print activer
			parser_print(ast);

		env->last_exit_code = execute_ast(ast); //execute l'ast et return l'exit code
		destroy_AST(ast);

		if (env->should_exit == 1)
			break;

		free_token(lexer->current);
		lexer = get_token(lexer); //passe au token suivant
	}

    int return_val = env->last_exit_code; //recup la derniere valeur d'exit
    
	free_lexer(lexer);
    fclose(file);

    return return_val;
}

int main(int argc,char* argv[])
{
    // Ensure random numbers for $RANDOM for two shells launched at the same
    // time
    srand(time(NULL) ^ getpid());
    env = init_env(argc, argv);
    int exit_code = my_42sh(argc,argv);


    //free env
    hash_map_free(env->variables, free);
	hash_map_free(env->functions, destroy_AST_void);
    free_export(env->export_variables);
    free_env(env);

    return exit_code;
}

