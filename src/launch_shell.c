#include "launch_shell.h"

extern struct env *env;

/**
 * Point d'entrée principal du shell 42sh.
 * Parse et exécute les commandes depuis l'input (fichier, stdin ou option -c).
 * Retourne le code de sortie de la dernière commande exécutée ou 2 en cas
 * d'erreur.
 */

int my_42sh(int argc, char *argv[])
{
    // PRETTY PRINT A ACTIVER AVEC PRETTY_PRINT=1 dans le terminal
    int pretty_print = 0;

    char *pretty_print_value = getenv("PRETTY_PRINT");

    if (pretty_print_value)
        pretty_print = (atoi(pretty_print_value) == 1);

    FILE *file = get_input_file(
        argc,
        argv); // IO_backend pour transformer ce qui est donner en arg en FILE

    if (file == NULL) // erreur IO
    {
        fprintf(stderr, "Error: IO\n");
        return 2;
    }

    struct lexer *lexer = init_lexer(file); // creer le lexer
    if (!lexer) // erreur de lexing
    {
        fprintf(stderr, "Error: Could not create the lexer struct\n");

        fclose(file);

        return 2;
    }

    while (lexer->current != NULL) // pour traiter tous les AST
    {
        struct AST *ast =
            input(&lexer); // fonction principale du parser (renvoi l'ast)

        if (ast == NULL) // erreur de Parsing
        {
            fprintf(stderr, "Error: Parsing\n");

            free_lexer(lexer);

            fclose(file);

            return 2;
        }

        if (pretty_print) // si pretty print activer
            parser_print(ast);

        env->last_exit_code =
            execute_ast(ast); // execute l'ast et return l'exit code
        destroy_AST(ast);

        if (env->should_exit == 1)
            break;

        free_token(lexer->current);
        lexer = get_token(lexer); // passe au token suivant
    }

    int return_val = env->last_exit_code; // recup la derniere valeur d'exit

    free_lexer(lexer);
    fclose(file);

    return return_val;
}
