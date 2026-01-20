#include "../parser.h"

//(20) prefix =
//        redirection
//        | ASSIGNEMENT_WORD

struct AST *prefix(struct token **token)
{
    if ((*token)->type == A_WORDS)//cas 2
    {
        struct AST *ast =
            create_ast(AST_ASSIGNEMENT, strdup((*token)->content));//recup la valeur du a_word
        *token = eat(*token);
        return ast;
    }

    else if (first_redirection(*token))//cas 1
    {
        struct AST *ast = redirection(token);
        if (ast == NULL)//remonte l'erreur
        {
            return NULL;
        }
        return ast;
    }

    return NULL;
}

//(21) redirection = [IONUMBER] ( '>' | '<' | '>>' | '>&' | '<&' | '>|' | '<>' )
// WORD

struct AST *redirection(struct token **token)
{
    struct AST *ast = NULL;

    if (*token == NULL || (*token)->type != REDIR){ //pas une redirection
        goto err;
    }

    ast = create_ast(AST_REDIR, NULL);
    struct AST *ast_val = create_ast(AST_VALUE, strdup((*token)->content));//prend la valeur du redir
    ast = add_children(ast, ast_val);

    *token = eat(*token);

    if (!is_valid_word(*token)){//pas un valid word
        goto err;
    }

    struct AST *ast_val2 = create_ast(AST_VALUE, strdup((*token)->content));//recup la valeur du word
    ast = add_children(ast, ast_val2);

    *token = eat(*token); //mange le word
    
    return ast;

err:
    destroy_AST(ast);
    return NULL;
}

//(22) element =
//        WORD
//        | redirection

struct AST *element(struct token **token)
{
    if (is_valid_word(*token))//cas 1
    {
        struct AST *ast = create_ast(AST_VALUE, strdup((*token)->content));//recup la valeur du word
        *token = eat(*token);//mange le word
        return ast;
    }

    if ((*token)->type == REDIR)//cas 2
    {
        struct AST *ast = redirection(token);
        if (ast == NULL)//remonte l'erreur
        {
            return NULL;
        }
        return ast;
    }

    return NULL;
}

