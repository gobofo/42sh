#include "../parser.h"

//         command =
//(8)    simple_command
//(9)    | shell_command {redirection}
//       | funcdec {redirection} 

struct AST *command(struct token **token)
{
    struct AST *ast=NULL;

    int _redir=0;
    if (first_shell_command(*token))//cas de shell command
    {
        _redir=1;//pour la suite pour evite de mettre un while pour chaque 
        ast = create_ast(AST_SHELL_CMD, NULL);

        struct AST *ast_shell = shell_command(token);

        if (ast_shell == NULL)//remonte l'erreur
          goto err;

        ast = add_children(ast, ast_shell);
    }

    else if((*token)->type==WORDS){//pour le funcdec

      struct token *look_ahead=next_token(NULL);//on regarde un devant

      if(look_ahead && look_ahead->type==L_PAREN){ //si c une parenthese c un funcdec

        _redir=1;//pareil que pour le shl_cmd

        ast = create_ast(AST_FUNC, strdup((*token)->content));
        struct AST *ast_func = funcdec(token);

        if (ast_func == NULL)
          goto err;

        ast = add_children(ast, ast_func);
      }

    }

    if(_redir==0){ //c une simple commande

      if(!first_simple_command(*token)) //verifie que c une simple commande
        goto err;

      return simple_command(token);
    }

    while (first_redirection(*token)) //mager toutes les redirections
    {

      struct AST *ast_redir = redirection(token);

      if (ast_redir == NULL)//remonte l'err
        goto err;

      ast = add_children(ast, ast_redir);
    }

    if (follow_command(*token))
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

struct AST *shell_command(struct token **token)
{
    if (first_rule_if(*token)) //cas if
    {
        struct AST *ast = rule_if(token);
        if (ast == NULL)
            return NULL;
        return ast;
    }
    else if (first_rule_while(*token))//cas while 
    {
        struct AST *ast = rule_while(token);
        if (ast == NULL)
            return NULL;
        return ast;
    }
    else if (first_rule_until(*token))//cas until
    {
        struct AST *ast = rule_until(token);
        if (ast == NULL)
            return NULL;
        return ast;
    }
    else if (first_rule_for(*token))//cas for
    {
        struct AST *ast = rule_for(token);
        if (ast == NULL)
            return NULL;
        return ast;
    }
    else if (*token != NULL && ((*token)->type == L_BRACE || (*token)->type == L_PAREN)){ //cas 1 et 2

        bool is_paren = (*token)->type == L_PAREN;

        *token = eat(*token);

        if (!first_compound_list(*token)){ //pas une coumpound list
            return NULL;
        }

        struct AST *ast = compound_list(token);

        if (ast == NULL){//remonte l'erreur
            return NULL;
        }

        if (*token == NULL || ((*token)->type != R_BRACE && (*token)->type != R_PAREN)){ //sans parenthese / bracket a la fin
            goto err;
        }

        if (((*token)->type == R_PAREN && !is_paren) || ((*token)->type == R_BRACE && is_paren)){ //pas le meme correspondance ( } ou { )
            goto err;
        }

        *token = eat(*token);

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
//(19)    | {prefix} WORD { element }

struct AST *simple_command(struct token **token)
{
    struct AST *ast = create_ast(AST_SIMPLE_CMD, NULL);

    int pref = 0;

    while (*token && first_prefix(*token)) // va manger tous les prefix du debut
    {
        pref = 1;

        struct AST *child = prefix(token);
        if (child == NULL)//remonte l'erreur
            goto err;
        ast = add_children(ast, child);
    }

    if (pref == 0 && *token == NULL) //pas de prefix et rien apres
    {
        goto err;
    }

    if (pref == 0 || (*token && is_valid_word(*token))) // regle 2
    {
        struct AST *child;

        if (!is_valid_word(*token)){//pas un mot alors c une erreur
            goto err;
        }
            
        child = create_ast(AST_VALUE, strdup((*token)->content)); //prend la valeur du WORDS

        ast = add_children(ast, child);

        *token = eat(*token);

        while (*token && (first_element(*token) || is_valid_word(*token))) //first de element ou un valid word car element peut etre just word
        {
            struct AST *child_el = element(token);
            if (child_el == NULL)//remonte l'err
                goto err;

            ast = add_children(ast, child_el);
        }

        if (!follow_simple_command(*token))//si ce qui suit n'est pas bon direct erreur
        {
            goto err;
        }
        
        return ast;
    }
    else //cas 1
    {
        if (!follow_simple_command(*token))//si ce qui suit n'est pas bon direct erreur
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

struct AST *funcdec(struct token **token){

    if (*token == NULL || (*token)->type != WORDS){ //pas un bon mot
        return NULL;
    }

    *token = eat(*token);

    if (*token == NULL || (*token)->type != L_PAREN){ //pas paren
        return NULL;
    }

    *token = eat(*token);

    if (*token == NULL || (*token)->type != R_PAREN){ //pas paren
        return NULL;
    }

    *token = eat(*token);

    eat_newlines(token);

    return shell_command(token);

}
