#include "../parser.h"

//(9) rule_if = 'if' compound_list 'then' compound_list [else_clause] 'fi'

struct AST *rule_if(struct token **token)
{

    struct AST *ast = create_ast(AST_IF, NULL);

    if ((*token)->type != IF){ //pas un if
        goto err;
    }
    
    *token = eat(*token);

    if (!first_compound_list(*token)){ //pas une compound list
        goto err;
    }

    struct AST *child = compound_list(token);
    if (child == NULL) //remonte l'err
    {
        goto err;
    }
    ast = add_children(ast, child);

    if (*token == NULL || (*token)->type != THEN)
    {
        goto err;
    }

    *token = eat(*token);

    if (!first_compound_list(*token)){ //pas une compound list
        goto err;
    }

    struct AST *child_second = compound_list(token); //envoi dans compound list
    if (child_second == NULL)
    {
        goto err;
    }
    ast = add_children(ast, child_second);

    if (first_else_clause(*token))
    { // esle clause existe
        struct AST *child_third = else_clause(token);
        if (child_third == NULL)
        {
            goto err;
        }
        ast = add_children(ast, child_third);
    }

    if (*token == NULL || (*token)->type != FI)
    { // verifie la GRAMMAR FIN
        goto err;
    }

    *token = eat(*token); //on mange le fi

    return ast;

err:
    destroy_AST(ast);
    return NULL;
}

// else_clause =
//(10)    'else' compound_list
//(11)    | 'elif' compound_list 'then' compound_list [else_clause]

struct AST *else_clause(struct token **token)
{
    struct AST *ast = NULL;

    if ((*token)->type == ELSE)//cas du else
    {
        *token = eat(*token);

        if (!first_compound_list(*token))
        {
            return NULL;
        }
        ast = compound_list(token);
        return ast;
    }
    else if ((*token)->type == ELIF)//cas elif mais au final c comme un if
    {
        ast = create_ast(AST_IF, NULL);
        *token = eat(*token);

        if (!first_compound_list(*token))//pas une compound list
        {
            goto err;
        }

        struct AST *child = compound_list(token);
        if (child == NULL)//remnte l'err
        {
            goto err;
        }
        ast = add_children(ast, child);

        if (*token == NULL || (*token)->type != THEN)//c pas then
        {
            goto err;
        }

        *token = eat(*token);//eat le then

        if (!first_compound_list(*token)){//pas un compound list
            goto err;
        }

        struct AST *child_second = compound_list(token);
        if (child_second == NULL)//remonte l'erreur
        {
            goto err;
        }
        ast = add_children(ast, child_second);

        if (first_else_clause(*token))
        { // esle clause existe
            struct AST *child_third = else_clause(token);
            if (child_third == NULL)//remonte l'err
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
