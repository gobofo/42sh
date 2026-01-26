#include "../parser.h"

//(9) rule_if = 'if' compound_list 'then' compound_list [else_clause] 'fi'

struct AST *rule_if(struct lexer **lexer)
{
    struct AST *ast = create_ast(AST_IF, NULL);

    if (get_current_type(*lexer) != IF)
    { // pas un if
        goto err;
    }

    *lexer = eat(*lexer);

    if (!first_compound_list(get_current_token(*lexer)))
    { // pas une compound list
        goto err;
    }

    struct AST *child = compound_list(lexer);
    if (child == NULL) // remonte l'err
    {
        goto err;
    }
    ast = add_children(ast, child);

    if (get_current_token(*lexer) == NULL || get_current_type(*lexer) != THEN)
    {
        goto err;
    }

    *lexer = eat(*lexer);

    if (!first_compound_list(get_current_token(*lexer)))
    { // pas une compound list
        goto err;
    }

    struct AST *child_second = compound_list(lexer); // envoi dans compound list
    if (child_second == NULL)
    {
        goto err;
    }
    ast = add_children(ast, child_second);

    if (first_else_clause(get_current_token(*lexer)))
    { // esle clause existe
        struct AST *child_third = else_clause(lexer);
        if (child_third == NULL)
        {
            goto err;
        }
        ast = add_children(ast, child_third);
    }

    if (get_current_token(*lexer) == NULL || get_current_type(*lexer) != FI)
    { // verifie la GRAMMAR FIN
        goto err;
    }

    *lexer = eat(*lexer); // on mange le fi

    return ast;

err:
    destroy_AST(ast);
    return NULL;
}

// else_clause =
//(10)    'else' compound_list
//(11)    | 'elif' compound_list 'then' compound_list [else_clause]

struct AST *else_clause(struct lexer **lexer)
{
    struct AST *ast = NULL;

    if (get_current_type(*lexer) == ELSE) // cas du else
    {
        *lexer = eat(*lexer);

        if (!first_compound_list(get_current_token(*lexer)))
        {
            return NULL;
        }
        ast = compound_list(lexer);
        return ast;
    }
    else if (get_current_type(*lexer)
             == ELIF) // cas elif mais au final c comme un if
    {
        ast = create_ast(AST_IF, NULL);
        *lexer = eat(*lexer);

        if (!first_compound_list(
                get_current_token(*lexer))) // pas une compound list
        {
            goto err;
        }

        struct AST *child = compound_list(lexer);
        if (child == NULL) // remnte l'err
        {
            goto err;
        }
        ast = add_children(ast, child);

        if (get_current_token(*lexer) == NULL
            || get_current_type(*lexer) != THEN) // c pas then
        {
            goto err;
        }

        *lexer = eat(*lexer); // eat le then

        if (!first_compound_list(get_current_token(*lexer)))
        { // pas un compound list
            goto err;
        }

        struct AST *child_second = compound_list(lexer);
        if (child_second == NULL) // remonte l'erreur
        {
            goto err;
        }
        ast = add_children(ast, child_second);

        if (first_else_clause(get_current_token(*lexer)))
        { // esle clause existe
            struct AST *child_third = else_clause(lexer);
            if (child_third == NULL) // remonte l'err
            {
                goto err;
            }
            ast = add_children(ast, child_third);
        }
        return ast;
    }

err:
    destroy_AST(ast);
    return NULL;
}
