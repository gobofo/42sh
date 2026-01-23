#include "../parser.h"

//         command =
//(8)    simple_command
//(9)    | shell_command {redirection}
//       | funcdec {redirection} 

struct AST *command(struct lexer **lexer)
{
    struct AST *ast=NULL;

    int _redir=0;
    if (first_shell_command(donne_token(*lexer)))//cas de shell command
    {
        _redir=1;//pour la suite pour evite de mettre un while pour chaque 
        ast = create_ast(AST_SHELL_CMD, NULL);

        struct AST *ast_shell = shell_command(lexer);

        if (ast_shell == NULL)//remonte l'erreur
          goto err;

        ast = add_children(ast, ast_shell);
    }

    else if(donne_type(*lexer)==WORDS){//pour le funcdec

      next_token(lexer);

      struct token *look_ahead = (*lexer)->next;//on regarde un devant

      if(look_ahead && look_ahead->type==L_PAREN){ //si c une parenthese c un funcdec

        _redir=1;//pareil que pour le shl_cmd

        ast = create_ast(AST_FUNC, strdup(donne_content(*lexer)));
        struct AST *ast_func = funcdec(lexer);

        if (ast_func == NULL)
          goto err;

        ast = add_children(ast, ast_func);
      }

    }

    if(_redir==0){ //c une simple commande

      if(!first_simple_command(donne_token(*lexer))) //verifie que c une simple commande
        goto err;

      return simple_command(lexer);
    }

    while (first_redirection(donne_token(*lexer))) //mager toutes les redirections
    {

      struct AST *ast_redir = redirection(lexer);

      if (ast_redir == NULL)//remonte l'err
        goto err;

      ast = add_children(ast, ast_redir);
    }

    if (follow_command(donne_token(*lexer)))
      return ast;

err:
    destroy_AST(ast);
    return NULL;
}

//(10) shell_c
//    '{' compound_list '}'
//   |'(' compound_list ')'
//   | rule_if
//   | rule_while
//   | rule_until
//   | rule_for

struct AST *shell_command(struct lexer **lexer)
{
    if (first_rule_if(donne_token(*lexer))) //cas if
    {
        struct AST *ast = rule_if(lexer);
        if (ast == NULL)
            return NULL;
        return ast;
    }
    else if (first_rule_while(donne_token(*lexer)))//cas while 
    {
        struct AST *ast = rule_while(lexer);
        if (ast == NULL)
            return NULL;
        return ast;
    }
    else if (first_rule_until(donne_token(*lexer)))//cas until
    {
        struct AST *ast = rule_until(lexer);
        if (ast == NULL)
            return NULL;
        return ast;
    }
    else if (first_rule_for(donne_token(*lexer)))//cas for
    {
        struct AST *ast = rule_for(lexer);
        if (ast == NULL)
            return NULL;
        return ast;
    }
    else if (donne_token(*lexer) != NULL && (donne_type(*lexer) == L_BRACE || donne_type(*lexer) == L_PAREN)){ //cas 1 et 2

        bool is_paren = donne_type(*lexer) == L_PAREN;

        *lexer = eat(*lexer);

        if (!first_compound_list(donne_token(*lexer))){ //pas une coumpound list
            return NULL;
        }

        struct AST *ast = compound_list(lexer);

        if (ast == NULL){//remonte l'erreur
            return NULL;
        }

        if (donne_token(*lexer) == NULL || (donne_type(*lexer) != R_BRACE && donne_type(*lexer) != R_PAREN)){ //sans parenthese / bracket a la fin
            goto err;
        }

        if ((donne_type(*lexer) == R_PAREN && !is_paren) || (donne_type(*lexer) == R_BRACE && is_paren)){ //pas le meme correspondance ( } ou { )
            goto err;
        }

        *lexer = eat(*lexer);
        if(is_paren)//subshell
        {
            struct AST *ast_sub_shell=create_ast(AST_SUB,NULL);
            ast=add_children(ast_sub_shell,ast);
        }

        return ast;
err:
        destroy_AST(ast);
        return NULL;
    }
    else
    {
        return NULL;
    }
}

//(18) simple_command =
//         prefix {prefix}
//(19)    | {prefix} ( WORD | SUBSHELL ) { element }

struct AST *simple_command(struct lexer **lexer)
{
    struct AST *ast = create_ast(AST_SIMPLE_CMD, NULL);

    int pref = 0;

    while (donne_token(*lexer) && first_prefix(donne_token(*lexer))) // va manger tous les prefix du debut
    {
        pref = 1;

        struct AST *child = prefix(lexer);
        if (child == NULL)//remonte l'erreur
            goto err;
        ast = add_children(ast, child);
    }

    if (pref == 0 && donne_token(*lexer) == NULL) //pas de prefix et rien apres
    {
        goto err;
    }

    if (pref == 0 || (donne_token(*lexer) && is_valid_word(*lexer))) // regle 2
    {
        struct AST *child;

        if (!is_valid_word(*lexer)){//pas un mot alors c une erreur
            goto err;
        }
            
        child = create_ast(AST_VALUE, strdup(donne_content(*lexer))); //prend la valeur du WORDS

        ast = add_children(ast, child);

        //pour gerer les subshell

        if (donne_type(*lexer) == SUBSHELL && !verif_subshell(*lexer)){

            goto err;

        }

        *lexer = eat(*lexer);

        while (donne_token(*lexer) && (first_element(donne_token(*lexer)) || is_valid_word(*lexer))) //first de element ou un valid word car element peut etre just word
        {
            struct AST *child_el = element(lexer);
            if (child_el == NULL)//remonte l'err
                goto err;

            ast = add_children(ast, child_el);
        }

        if (!follow_simple_command(donne_token(*lexer)))//si ce qui suit n'est pas bon direct erreur
        {
            goto err;
        }
        
        return ast;
    }
    else //cas 1
    {
        if (!follow_simple_command(donne_token(*lexer)))//si ce qui suit n'est pas bon direct erreur
        {
            goto err;
        }

        return ast;
    }

err:
    destroy_AST(ast);
    return NULL;
}

//funcdec = WORD '(' ')' {'\n'} shell_command ;

struct AST *funcdec(struct lexer **lexer){

    if (donne_token(*lexer) == NULL || donne_type(*lexer) != WORDS){ //pas un bon mot
        return NULL;
    }

    *lexer = eat(*lexer);

    if (donne_token(*lexer) == NULL || donne_type(*lexer) != L_PAREN){ //pas paren
        return NULL;
    }

    *lexer = eat(*lexer);

    if (donne_token(*lexer) == NULL || donne_type(*lexer) != R_PAREN){ //pas paren
        return NULL;
    }

    *lexer = eat(*lexer);

    eat_newlines(lexer);

    return shell_command(lexer);

}
