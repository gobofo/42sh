#include "../parser.h"

//    input =
//(1)     list '\n'
//(2)    | list EOF
//(3)    | '\n'
//(4)    | EOF

struct AST *input(struct lexer **lexer)
{
    if (get_current_token(*lexer) == NULL)  //regle 4
    {
        return create_ast(AST_LIST, NULL);
    }

    if (get_current_type(*lexer) == NEWLINE){ //regle 3

        return create_ast(AST_LIST, NULL);

    }

    if (!first_list(get_current_token(*lexer))){ //si c pas une liste = erreur
        return NULL;
    }

    struct AST *ast = list(lexer); //creer la list
    if (ast == NULL) //pb dans la list
    {
        return NULL;
    }

    if (!follow_list(get_current_token(*lexer))) //pas \n ou EOF
    {
        destroy_AST(ast);
        return NULL;
    }

    return ast;
}

//(3) list = and_or { ';' and_or } [ ';' ]

struct AST *list(struct lexer **lexer)
{
    struct AST *ast = create_ast(AST_LIST, NULL);

    if (!first_and_or(get_current_token(*lexer))){//pas debut du and_or
        goto err;
    }

    struct AST *child = and_or(lexer);

    if (child == NULL){//remonte le pb
        goto err;
    }

    ast = add_children(ast, child);

    if (follow_list(get_current_token(*lexer))){//pas de ;
        return ast;
    }

    if (get_current_type(*lexer) == SEMICOLON){
        *lexer = eat(*lexer);

        if (follow_list(get_current_token(*lexer))){ // cas [ ; ]
            return ast;
        }
    }

    else { //pas de semicolon
        goto err;
    }

    while (first_and_or(get_current_token(*lexer))){ //first de and_or

        struct AST *child = and_or(lexer);
        if (child == NULL) //err remonter
        {
            goto err;
        }
        ast = add_children(ast, child);

        if (follow_list(get_current_token(*lexer)))//fin de la liste
        {
            return ast;
        }

        if (get_current_type(*lexer) == SEMICOLON)//si ya ;
        {
            *lexer = eat(*lexer);
        }

        if (follow_list(get_current_token(*lexer)))//fin de la liste
        {
            return ast;
        }

    }

err:
    destroy_AST(ast);
    return NULL;
}

//(6) and_or = pipeline { ( '&&' | '||' ) {'\n'} pipeline }

struct AST *and_or(struct lexer **lexer)
{
    
    struct AST *ast = NULL;

    if (!first_pipeline(get_current_token(*lexer))){//pas un pipeline
        goto err;
    }

    ast = pipeline(lexer);

    if (ast == NULL){//remonte l'erreur
        goto err;
    }

    while (get_current_token(*lexer) && (get_current_type(*lexer) == AND || get_current_type(*lexer) == OR)){

        struct AST *ast_op;

        if (get_current_type(*lexer) == AND)
        {
            ast_op = create_ast(AST_AND, NULL); //le AND 
        }
        else
        {
            ast_op = create_ast(AST_OR, NULL); //le OR
        }

        *lexer = eat(*lexer);

        eat_newlines(lexer); //pour {\n}

        if (!first_pipeline(get_current_token(*lexer))){
            destroy_AST(ast_op);
            goto err;
        }

        ast_op = add_children(ast_op, ast);
        ast = pipeline(lexer);

        if (ast == NULL) //remonte l'erreur
        {
            destroy_AST(ast_op);
            goto err;
        }

        ast_op = add_children(ast_op, ast);
        ast = ast_op; // on change comme ca les AST de and et OR se construisent a l'envers

    }

    if (follow_and_or(get_current_token(*lexer))){
        return ast;
    }

err:
    destroy_AST(ast);
    return NULL;
}

//(7) pipeline = [!] command { '|' {'\n'} command }

struct AST *pipeline(struct lexer **lexer)
{
    struct AST *ast = create_ast(AST_PIPELINE, NULL);
    if (get_current_type(*lexer) == NEG)
    {
        ast->is_neg = 1;
        *lexer = eat(*lexer);
    }

    if (!first_command(get_current_token(*lexer))){ //pas une commande
        goto err;
    }

    struct AST *children = command(lexer);

    if (children == NULL) //remonte l'erreur 
    {
        goto err;
    }

    ast = add_children(ast, children);

    if (follow_pipeline(get_current_token(*lexer))){
        return ast;
    }

    while (get_current_type(*lexer) == PIPE)
    {
        *lexer = eat(*lexer);
        if (get_current_token(*lexer) == NULL)
            goto err;

        eat_newlines(lexer);

        if (!first_command(get_current_token(*lexer))){
            goto err;
        }

        struct AST *children = command(lexer);

        if (children == NULL) //remonte l'erreur
        {
            goto err;
        }

        ast = add_children(ast, children);

        if (follow_pipeline(get_current_token(*lexer))){
            return ast;
        }
    }

err:
    destroy_AST(ast);
    return NULL;
}

//(17) compound_list = {'\n'} and_or { ( ';' | '\n' ) {'\n'} and_or } [';'] {'\n'}

struct AST *compound_list(struct lexer **lexer)
{
    struct AST *ast = create_ast(AST_LIST, NULL);

    eat_newlines(lexer);//pour les {\n}

    if (!first_and_or(get_current_token(*lexer))) //pas un and or
        goto err;

    struct AST *child = and_or(lexer);
    if (child == NULL)//remonte l'err
        goto err;

    ast = add_children(ast, child);
    
    while (get_current_token(*lexer) != NULL
           && (get_current_type(*lexer) == SEMICOLON || get_current_type(*lexer) == NEWLINE))
    {
        *lexer = eat(*lexer); //mange le /n ou le ;
        eat_newlines(lexer); //pour le {\n}

        if (get_current_token(*lexer) == NULL || follow_compound_list(get_current_token(*lexer)))//c la fin
            break;

        if (!first_and_or(get_current_token(*lexer))) //c pas un and or
            goto err;

        child = and_or(lexer);
        if (child == NULL)//remonte l'err
            goto err;
        ast = add_children(ast, child);
    }

    //==================== PAS UTILE =========================

    if (get_current_token(*lexer) != NULL && get_current_type(*lexer) == SEMICOLON)
        *lexer = eat(*lexer);

    eat_newlines(lexer);

    //==================== PAS UTILE =========================

    return ast;

err:
    destroy_AST(ast);
    return NULL;
}

