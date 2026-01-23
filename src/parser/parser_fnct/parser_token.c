#include "../parser.h"

//(20) prefix =
//        redirection
//        | ASSIGNEMENT_WORD

struct AST *prefix(struct lexer **lexer)
{
    if (get_current_type(*lexer) == A_WORDS)//cas 2
    {
        struct AST *ast =
            create_ast(AST_ASSIGNEMENT, strdup(get_current_content(*lexer)));//recup la valeur du a_word
        *lexer = eat(*lexer);
        return ast;
    }

    else if (first_redirection(get_current_token(*lexer)))//cas 1
    {
        struct AST *ast = redirection(lexer);
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

struct AST *redirection(struct lexer **lexer)
{
    struct AST *ast = NULL;

    if (get_current_token(*lexer) == NULL || get_current_type(*lexer) != REDIR){ //pas une redirection
        goto err;
    }

    ast = create_ast(AST_REDIR, NULL);
    struct AST *ast_val = create_ast(AST_VALUE, strdup(get_current_content(*lexer)));//prend la valeur du redir
    ast = add_children(ast, ast_val);

    *lexer = eat(*lexer);

    if (!is_valid_word(*lexer)){//pas un valid word
        goto err;
    }

    struct AST *ast_val2 = create_ast(AST_VALUE, strdup(get_current_content(*lexer)));//recup la valeur du word
    ast = add_children(ast, ast_val2);

    *lexer = eat(*lexer); //mange le word
    
    return ast;

err:
    destroy_AST(ast);
    return NULL;
}

//(22) element =
//        ( WORD | SUBSHELL )
//        | redirection

struct AST *element(struct lexer **lexer)
{
    if (is_valid_word(*lexer))//cas 1
    {
        struct AST *ast = create_ast(AST_VALUE, strdup(get_current_content(*lexer)));//recup la valeur du word

        //pour gerer les subshell

        if (get_current_type(*lexer) == SUBSHELL && !verif_subshell(*lexer)){

            destroy_AST(ast);
            return NULL;

        }

        *lexer = eat(*lexer);//mange le word
        return ast;
    }

    if (get_current_type(*lexer) == REDIR)//cas 2
    {
        struct AST *ast = redirection(lexer);
        if (ast == NULL)//remonte l'erreur
        {
            return NULL;
        }
        return ast;
    }

    return NULL;
}

