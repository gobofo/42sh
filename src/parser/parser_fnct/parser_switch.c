#include "../parser.h"

//rule_case = 'case' WORD {'\n'} 'in' {'\n'} [case_clause] 'esac'

struct AST *rule_case(struct lexer **lexer){

    struct AST *ast = create_ast(AST_CASE, NULL);

    if (!get_current_token(*lexer) || get_current_type(*lexer) != CASE){// differnet de case

        goto err;

    }

    *lexer = eat(*lexer);//mange le case

    if (!is_valid_word(*lexer)){//pas un vrai mot
        goto err;
    }

    ast->content = strdup(get_current_content(*lexer));
    *lexer = eat(*lexer);//mange le case


    eat_newlines(lexer); //mange les nouvelle lignes

    if (!get_current_token(*lexer) || get_current_type(*lexer) != IN){//pas de in
        goto err;
    }

    *lexer = eat(*lexer);//mange le case


    eat_newlines(lexer);//mange {\n}

    if (first_case_clause(get_current_token(*lexer))){ //si case_clause
        struct AST *children = case_clause(lexer);

        if (children == NULL){
            goto err;
        }

        ast = add_children(ast, children);
    }

    if (!get_current_token(*lexer) || !(get_current_type(*lexer) == ESAC)){ //pas de esac

        goto err;

    }
    
    *lexer = eat(*lexer);//mange le case
    return ast;

err:
    destroy_AST(ast);
    return NULL;

}

//case_clause = case_item { ';;' {'\n'} case_item } [';;'] {'\n'}

struct AST *case_clause(struct lexer **lexer){

    struct AST *ast = create_ast(AST_CASE_ITEM, NULL);
    
    if (!(first_case_item(get_current_token(*lexer)))){
        goto err;
    }

    struct AST *child = case_item(lexer);

    if (child == NULL){

        goto err;

    }

    ast = add_children(ast, child); // premier child


    while (get_current_token(*lexer) && get_current_type(*lexer) == D_SEMICOLON){

        *lexer = eat(*lexer);

        /*if (get_current_token(*lexer)){ //vide
            goto err;
        }
        */

        eat_newlines(lexer);

        if (follow_case_clause(get_current_token(*lexer))){ //fin de la grammaire
            return ast;
        }

        if (!first_case_item(get_current_token(*lexer))){
            goto err;
        }

        struct AST *children = case_item(lexer);

        if (children == NULL){
            goto err;
        }

        ast = add_children(ast, children);

    }

err:
    destroy_AST(ast);
    return NULL;

}

//This fucntion is call inside case_item
//It's use to handle the case { '|' WORD }
//if there is an issue it return -1
//else it return 0

static int parse_multi_word_case(struct lexer **lexer, struct AST *ast){

    while (get_current_token(*lexer) && get_current_type(*lexer) == PIPE){

        *lexer = eat(*lexer);//mange le |

        if (!is_valid_word(*lexer)){ //pas un valid word
            return -1;
        }

        struct AST *child = create_ast(AST_VALUE, strdup(get_current_content(*lexer)));

        ast = add_children(ast, child); //ajoute en value le word

        *lexer = eat(*lexer); //mange ke word

    }

    return 0;

}

//case_item = ['('] WORD { '|' WORD } ')' {'\n'} [compound_list]

struct AST *case_item(struct lexer **lexer){

    struct AST *ast = create_ast(AST_CASE_ITEM, NULL);

    if (!get_current_token(*lexer)){//token NULL
        goto err;
    }

    if (get_current_type(*lexer) == L_PAREN){//si parenthese
        *lexer = eat(*lexer);
    }

    if (!is_valid_word(*lexer)){//pas un valid word
        goto err;
    }

    struct AST *child = create_ast(AST_VALUE, strdup(get_current_content(*lexer)));
    ast = add_children(ast, child);

    *lexer = eat(*lexer); //mange le word

    if (parse_multi_word_case(lexer, ast) == -1 ){ //gere les cas { '|' WORD }
        goto err;
    }

    if (!get_current_token(*lexer) || get_current_type(*lexer) != R_PAREN){ //pas de )
        goto err;
    }

    *lexer = eat(*lexer);//mange la )

    eat_newlines(lexer);// mange les {\n}

    if (follow_case_item(get_current_token(*lexer))){ //pas de coumpound_list
        return ast;
    }

    //a partir d'ici ya une compound list

    if (!first_compound_list(get_current_token(*lexer))){ //pas un first de compound list
        goto err;
    }

    struct AST *children = compound_list(lexer); //creer l'arbre compound list

    if (children == NULL){ //err dans compound list
        goto err;
    }

    ast = add_children(ast, children); 

    return ast;

err:
    destroy_AST(ast);
    return NULL;

}





