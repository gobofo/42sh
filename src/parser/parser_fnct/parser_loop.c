#include "../parser.h"

//(14) rule_while = 'while' compound_list 'do' compound_list 'done'

struct AST *rule_while(struct lexer **lexer)
{
    struct AST *ast = create_ast(AST_WHILE, NULL);

    if (get_current_type(*lexer) != WHILE)
    { // pas de while au debut
        goto err;
    }

    *lexer = eat(*lexer); /// mange le while

    if (!first_compound_list(get_current_token(*lexer)))
    { // pas un first de compound list
        goto err;
    }

    struct AST *child = compound_list(lexer);

    if (child == NULL)
    { // remonte l'erreur
        goto err;
    }

    ast = add_children(ast, child);

    if (get_current_token(*lexer) == NULL
        || get_current_type(*lexer) != DO) // pas de DO
        goto err;

    *lexer = eat(*lexer);

    if (!first_compound_list(get_current_token(*lexer)))
    { // pas un first de compound list
        goto err;
    }

    struct AST *child_second = compound_list(lexer);

    if (child_second == NULL)
    { // remonte l'erreur
        goto err;
    }

    ast = add_children(ast, child_second);

    if (get_current_token(*lexer) == NULL
        || get_current_type(*lexer) != DONE) // pas de done
        goto err;

    *lexer = eat(*lexer);

    return ast;

err:
    destroy_AST(ast);
    return NULL;
}

//(15) rule_until = 'until' compound_list 'do' compound_list 'done'

struct AST *rule_until(struct lexer **lexer)
{
    struct AST *ast = create_ast(AST_UNTIL, NULL);

    if (get_current_type(*lexer) != UNTIL)
    { // pas de unitl au debut
        goto err;
    }

    *lexer = eat(*lexer); /// mange le until

    if (!first_compound_list(get_current_token(*lexer)))
    { // pas un first de compound list
        goto err;
    }

    struct AST *child = compound_list(lexer);

    if (child == NULL)
    { // remonte l'erreur
        goto err;
    }

    ast = add_children(ast, child);

    if (get_current_token(*lexer) == NULL
        || get_current_type(*lexer) != DO) // pas de DO
        goto err;

    *lexer = eat(*lexer);

    if (!first_compound_list(get_current_token(*lexer)))
    { // pas un first de compound list
        goto err;
    }

    struct AST *child_second = compound_list(lexer);

    if (child_second == NULL)
    { // remonte l'erreur
        goto err;
    }

    ast = add_children(ast, child_second);

    if (get_current_token(*lexer) == NULL
        || get_current_type(*lexer) != DONE) // pas de done
        goto err;

    *lexer = eat(*lexer);

    return ast;

err:
    destroy_AST(ast);
    return NULL;
}

// this function is here to parse the body of the for
//( [';'] | [ {'\n'} 'in' { ( WORD | SUBSHELL) } ( ';' | '\n') ] )
// it return -1 if the parsing is bad
// and ut return 0 if the parsing is good

static int parse_for_body(struct lexer **lexer, struct AST *ast)
{
    // Cas 1
    if (get_current_type(*lexer) == SEMICOLON)
    {
        *lexer = eat(*lexer);
    }
    else // Cas 2
    {
        eat_newlines(lexer); // enleve le {\n}

        if (get_current_token(*lexer) != NULL
            && get_current_type(*lexer) == IN) // cas avec le in
        {
            *lexer = eat(*lexer);

            if (get_current_token(*lexer) == NULL) // eviter pb
                return -1;

            while (get_current_token(*lexer) != NULL
                   && get_current_type(*lexer) != SEMICOLON
                   && get_current_type(*lexer)
                       != NEWLINE) // va mager tous les words
            {
                if (!is_valid_word(*lexer)) // c pas un mots alors c une err
                    return -1;

                struct AST *word =
                    create_ast(AST_VALUE,
                               strdup(get_current_content(
                                   *lexer))); // prend la valeur du mot
                ast = add_children(ast, word);

                // pour gerer les subshell

                if (get_current_type(*lexer) == SUBSHELL
                    && !verif_subshell(*lexer))
                {
                    return -1;
                }

                *lexer = eat(*lexer);
            }

            if (get_current_token(*lexer) == NULL
                || (get_current_type(*lexer) != SEMICOLON
                    && get_current_type(*lexer)
                        != NEWLINE)) // problem de grammaire
                return -1;

            *lexer = eat(*lexer); // on eat le ; ou le \n
        }
    }

    return 0;
}

//(16) rule_for = 'for' WORD ( [';']
//                          | [ {'\n'} 'in' { ( WORD | SUBSHELL) } ( ';' | '\n'
//                          ) ] )
//{'\n'} 'do' compound_list 'done'

struct AST *rule_for(struct lexer **lexer)
{
    struct AST *ast = create_ast(AST_FOR, NULL);

    if (get_current_type(*lexer) != FOR) // pas de for au debut
        goto err;

    *lexer = eat(*lexer); // mange le for

    if (!is_valid_word(*lexer)) // pas bon word mais peut etre if, do (voir
                                // fonction is_valid_word)
        goto err;

    struct AST *var_name = create_ast(
        AST_VALUE, strdup(get_current_content(*lexer))); // nom de la variable
    ast = add_children(ast, var_name);
    *lexer = eat(*lexer);

    if (get_current_token(*lexer) == NULL) // si ya rien c erreur
        goto err;

    if (parse_for_body(lexer, ast) == -1)
    { // gere tous le body du for
        goto err;
    }

    eat_newlines(lexer); // enleve le {\n}
    if (get_current_token(*lexer) == NULL
        || get_current_type(*lexer) != DO) // pas de do
        goto err;

    *lexer = eat(*lexer); // mange le do

    if (!first_compound_list(
            get_current_token(*lexer))) // pas first de compound lst
        goto err;

    struct AST *body = compound_list(lexer);
    if (body == NULL) // remonte l'erreur
        goto err;

    ast = add_children(ast, body);

    if (get_current_token(*lexer) == NULL
        || get_current_type(*lexer) != DONE) // pas de done
        goto err;

    *lexer = eat(*lexer); // mange le done

    return ast;

err:
    destroy_AST(ast);
    return NULL;
}
