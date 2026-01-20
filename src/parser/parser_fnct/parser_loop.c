#include "../parser.h"

//(14) rule_while = 'while' compound_list 'do' compound_list 'done'

struct AST *rule_while(struct token **token)
{
    struct AST *ast = create_ast(AST_WHILE, NULL);

    if ((*token)->type != WHILE){//pas de while au debut
        goto err;
    }

    *token = eat(*token);///mange le while

    if (!first_compound_list(*token)){//pas un first de compound list
        goto err;
    }

    struct AST *child = compound_list(token);

    if (child == NULL){//remonte l'erreur
        goto err;
    }

    ast = add_children(ast, child);

    if (*token == NULL || (*token)->type != DO) // pas de DO
        goto err;

    *token = eat(*token);

    if (!first_compound_list(*token)){//pas un first de compound list
        goto err;
    }

    struct AST *child_second = compound_list(token);

    if (child_second == NULL){//remonte l'erreur
        goto err;
    }

    ast = add_children(ast, child_second);

    if (*token == NULL || (*token)->type != DONE) // pas de done
        goto err;

    *token = eat(*token);

    return ast;

err:
    destroy_AST(ast);
    return NULL;
}

//(15) rule_until = 'until' compound_list 'do' compound_list 'done'

struct AST *rule_until(struct token **token)
{
    struct AST *ast = create_ast(AST_UNTIL, NULL);

    if ((*token)->type != UNTIL){//pas de unitl au debut
        goto err;
    }

    *token = eat(*token);///mange le until

    if (!first_compound_list(*token)){//pas un first de compound list
        goto err;
    }

    struct AST *child = compound_list(token);

    if (child == NULL){//remonte l'erreur
        goto err;
    }

    ast = add_children(ast, child);

    if (*token == NULL || (*token)->type != DO) // pas de DO
        goto err;

    *token = eat(*token);

    if (!first_compound_list(*token)){//pas un first de compound list
        goto err;
    }

    struct AST *child_second = compound_list(token);

    if (child_second == NULL){//remonte l'erreur
        goto err;
    }

    ast = add_children(ast, child_second);

    if (*token == NULL || (*token)->type != DONE) // pas de done
        goto err;

    *token = eat(*token);

    return ast;

err:
    destroy_AST(ast);
    return NULL;
}

//(16) rule_for = 'for' WORD ( [';']
//                          | [ {'\n'} 'in' { WORD } ( ';' | '\n' ) ] )
//{'\n'} 'do' compound_list 'done'

struct AST *rule_for(struct token **token)
{
    struct AST *ast = create_ast(AST_FOR, NULL);

    if ((*token)->type != FOR) //pas de for au debut
        goto err;

    *token = eat(*token); //mange le for

    if (!is_valid_word(*token)) //pas bon word mais peut etre if, do (voir fonction is_valid_word)
        goto err;

    struct AST *var_name = create_ast(AST_VALUE, strdup((*token)->content)); //nom de la variable
    ast = add_children(ast, var_name);
    *token = eat(*token);

    if (*token == NULL)//si ya rien c erreur
        goto err;

    // Cas 1
    if ((*token)->type == SEMICOLON)
    {
        *token = eat(*token);
    }
    else // Cas 2
    {
        eat_newlines(token); //enleve le {\n}

        if (*token != NULL && (*token)->type == IN) //cas avec le in
        {
            *token = eat(*token);

            if (*token == NULL)//eviter pb
                goto err;

            while (*token != NULL && (*token)->type != SEMICOLON
                   && (*token)->type != NEWLINE) //va mager tous les words
            {

                if (!is_valid_word(*token)) //c pas un mots alors c une err
                    goto err;

                struct AST *word =
                    create_ast(AST_VALUE, strdup((*token)->content)); //prend la valeur du mot
                ast = add_children(ast, word);

                *token = eat(*token);
            }

            if (*token == NULL
                || ((*token)->type != SEMICOLON && (*token)->type != NEWLINE)) //problem de grammaire
                goto err;

            *token = eat(*token);//on eat le ; ou le \n
        }
    }

    eat_newlines(token);//enleve le {\n}
    if (*token == NULL || (*token)->type != DO) //pas de do
        goto err;

    *token = eat(*token);//mange le do

    if (!first_compound_list(*token))//pas first de compound lst
        goto err;

    struct AST *body = compound_list(token);
    if (body == NULL)//remonte l'erreur
        goto err;

    ast = add_children(ast, body);

    if (*token == NULL || (*token)->type != DONE) //pas de done
        goto err;

    *token = eat(*token); //mange le done

    return ast;

err:
    destroy_AST(ast);
    return NULL;
}
