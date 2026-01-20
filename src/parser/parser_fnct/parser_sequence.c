#include "../parser.h"

//    input =
//(1)     list '\n'
//(2)    | list EOF
//(3)    | '\n'
//(4)    | EOF

struct AST *input(struct token **token)
{
    if (*token == NULL || (*token)->type == NEWLINE)  //regle 3 et 4
    {
        free_token(*token);
        return create_ast(AST_LIST, NULL);
    }

    if (!first_list(*token)){ //si c pas une liste = erreur
        free_token(*token);
        return NULL;
    }

    struct AST *ast = list(token); //creer la list
    if (ast == NULL) //pb dans la list
    {
        free_token(*token);
        return NULL;
    }

    if (!follow_list(*token)) //pas \n ou EOF
    {
        free_token(*token);
        destroy_AST(ast);
        return NULL;
    }

    free_token(*token); // si \NEWLINE
    return ast;
}

//(3) list = and_or { ';' and_or } [ ';' ]

struct AST *list(struct token **token)
{
    struct AST *ast = create_ast(AST_LIST, NULL);

    if (!first_and_or(*token)){//pas debut du and_or
        goto err;
    }

    struct AST *child = and_or(token);

    if (child == NULL){//remonte le pb
        goto err;
    }

    ast = add_children(ast, child);

    if (follow_list(*token)){//pas de ;
        return ast;
    }

    if ((*token)->type == SEMICOLON){
        *token = eat(*token);

        if (follow_list(*token)){ // cas [ ; ]
            return ast;
        }
    }

    else { //pas de semicolon
        goto err;
    }

    while (first_and_or(*token)){ //first de and_or

        struct AST *child = and_or(token);
        if (child == NULL) //err remonter
        {
            goto err;
        }
        ast = add_children(ast, child);

        if (follow_list(*token))//fin de la liste
        {
            return ast;
        }

        if ((*token)->type == SEMICOLON)//si ya ;
        {
            *token = eat(*token);
        }

        if (follow_list(*token))//fin de la liste
        {
            return ast;
        }

    }

err:
    destroy_AST(ast);
    return NULL;
}

//(6) and_or = pipeline { ( '&&' | '||' ) {'\n'} pipeline }

struct AST *and_or(struct token **token)
{
    
    struct AST *ast = NULL;

    if (!first_pipeline(*token)){//pas un pipeline
        goto err;
    }

    ast = pipeline(token);

    if (ast == NULL){//remonte l'erreur
        goto err;
    }

    while (*token && ((*token)->type == AND || (*token)->type == OR)){

        struct AST *ast_op;

        if ((*token)->type == AND)
        {
            ast_op = create_ast(AST_AND, NULL); //le AND 
        }
        else
        {
            ast_op = create_ast(AST_OR, NULL); //le OR
        }

        *token = eat(*token);

        eat_newlines(token); //pour {\n}

        if (!first_pipeline(*token)){
            destroy_AST(ast_op);
            goto err;
        }

        ast_op = add_children(ast_op, ast);
        ast = pipeline(token);

        if (ast == NULL) //remonte l'erreur
        {
            destroy_AST(ast_op);
            goto err;
        }

        ast_op = add_children(ast_op, ast);
        ast = ast_op; // on change comme ca les AST de and et OR se construisent a l'envers

    }

    if (follow_and_or(*token)){
        return ast;
    }

err:
    destroy_AST(ast);
    return NULL;
}

//(7) pipeline = [!] command { '|' {'\n'} command }

struct AST *pipeline(struct token **token)
{
    struct AST *ast = create_ast(AST_PIPELINE, NULL);
    if ((*token)->type == NEG)
    {
        ast->is_neg = 1;
        *token = eat(*token);
    }

    if (!first_command(*token)){ //pas une commande
        goto err;
    }

    struct AST *children = command(token);

    if (children == NULL) //remonte l'erreur 
    {
        goto err;
    }

    ast = add_children(ast, children);

    if (follow_pipeline(*token)){
        return ast;
    }

    while ((*token)->type == PIPE)
    {
        *token = eat(*token);
        if (*token == NULL)
            goto err;

        eat_newlines(token);

        if (!first_command(*token)){
            goto err;
        }

        struct AST *children = command(token);

        if (children == NULL) //remonte l'erreur
        {
            goto err;
        }

        ast = add_children(ast, children);

        if (follow_pipeline(*token)){
            return ast;
        }
    }

err:
    destroy_AST(ast);
    return NULL;
}

//(17) compound_list = {'\n'} and_or { ( ';' | '\n' ) {'\n'} and_or } [';'] {'\n'}

struct AST *compound_list(struct token **token)
{
    struct AST *ast = create_ast(AST_LIST, NULL);

    eat_newlines(token);//pour les {\n}

    if (!first_and_or(*token)) //pas un and or
        goto err;

    struct AST *child = and_or(token);
    if (child == NULL)//remonte l'err
        goto err;

    ast = add_children(ast, child);
    
    while (*token != NULL
           && ((*token)->type == SEMICOLON || (*token)->type == NEWLINE))
    {
        *token = eat(*token); //mange le /n ou le ;
        eat_newlines(token); //pour le {\n}

        if (*token == NULL || follow_compound_list(*token))//c la fin
            break;

        if (!first_and_or(*token)) //c pas un and or
            goto err;

        child = and_or(token);
        if (child == NULL)//remonte l'err
            goto err;
        ast = add_children(ast, child);
    }

    //==================== PAS UTILE =========================

    if (*token != NULL && (*token)->type == SEMICOLON)
        *token = eat(*token);

    eat_newlines(token);

    //==================== PAS UTILE =========================

    return ast;

err:
    destroy_AST(ast);
    return NULL;
}

