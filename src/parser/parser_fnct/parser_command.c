#include "../parser.h"

//         command =
//(8)    simple_command
//(9)    | shell_command {redirection}
//       | funcdec {redirection}

struct AST *command(struct lexer **lexer)
{
    get_alias_token(*lexer);

    if (get_current_token(*lexer) == NULL)
        return NULL;

    struct AST *ast = NULL;

    int _redir = 0;

    if (first_shell_command(get_current_token(*lexer))) // cas de shell command
    {
        _redir = 1; // pour la suite pour evite de mettre un while pour chaque
        ast = create_ast(AST_SHELL_CMD, NULL);

        struct AST *ast_shell = shell_command(lexer);

        if (ast_shell == NULL) // remonte l'erreur
            goto err;

        ast = add_children(ast, ast_shell);
    }

    else if (get_current_type(*lexer) == WORDS)
    { // pour le funcdec

        next_token(lexer);

        struct token *look_ahead = (*lexer)->next; // on regarde un devant

        if (look_ahead && look_ahead->type == L_PAREN)
        { // si c une parenthese c un funcdec

            _redir = 1; // pareil que pour le shl_cmd

            ast = create_ast(AST_FUNC, strdup(get_current_content(*lexer)));
            struct AST *ast_func = funcdec(lexer);

            if (ast_func == NULL)
                goto err;

            ast = add_children(ast, ast_func);
        }
    }

    if (_redir == 0)
    { // c une simple commande

        if (!first_simple_command(
                get_current_token(*lexer))) // verifie que c une simple commande
            goto err;

        return simple_command(lexer);
    }

    while (first_redirection(
        get_current_token(*lexer))) // mager toutes les redirections
    {
        struct AST *ast_redir = redirection(lexer);

        if (ast_redir == NULL) // remonte l'err
            goto err;

        ast = add_children(ast, ast_redir);
    }

    if (follow_command(get_current_token(*lexer)))
        return ast;

err:
    destroy_AST(ast);
    return NULL;
}

// this fnct is here to handle the case 1 and 2
// of shell command
//'{' compound_list '}' and '(' compound_list ')'

static struct AST *handle_subshell(struct lexer **lexer)
{
    bool is_paren = get_current_type(*lexer) == L_PAREN;

    *lexer = eat(*lexer);

    if (!first_compound_list(get_current_token(*lexer)))
    { // pas une coumpound list
        return NULL;
    }

    struct AST *ast = compound_list(lexer);

    if (ast == NULL)
    { // remonte l'erreur
        return NULL;
    }

    if (get_current_token(*lexer) == NULL
        || (get_current_type(*lexer) != R_BRACE
            && get_current_type(*lexer) != R_PAREN))
    { // sans parenthese / bracket a la fin
        goto err;
    }

    if ((get_current_type(*lexer) == R_PAREN && !is_paren)
        || (get_current_type(*lexer) == R_BRACE && is_paren))
    { // pas le meme correspondance ( } ou { )
        goto err;
    }

    *lexer = eat(*lexer);
    if (is_paren) // subshell
    {
        struct AST *ast_sub_shell = create_ast(AST_SUB, NULL);
        ast = add_children(ast_sub_shell, ast);
    }

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
    if (first_rule_if(get_current_token(*lexer))) // cas if
    {
        struct AST *ast = rule_if(lexer);
        if (ast == NULL)
            return NULL;
        return ast;
    }
    else if (first_rule_while(get_current_token(*lexer))) // cas while
    {
        struct AST *ast = rule_while(lexer);
        if (ast == NULL)
            return NULL;
        return ast;
    }
    else if (first_rule_until(get_current_token(*lexer))) // cas until
    {
        struct AST *ast = rule_until(lexer);
        if (ast == NULL)
            return NULL;
        return ast;
    }
    else if (first_rule_for(get_current_token(*lexer))) // cas for
    {
        struct AST *ast = rule_for(lexer);
        if (ast == NULL)
            return NULL;
        return ast;
    }
    else if (first_rule_case(get_current_token(*lexer))) // cas for
    {
        struct AST *ast = rule_case(lexer);
        if (ast == NULL)
            return NULL;
        return ast;
    }
    else if (get_current_token(*lexer) != NULL
             && (get_current_type(*lexer) == L_BRACE
                 || get_current_type(*lexer) == L_PAREN))
    { // cas 1 et 2

        return handle_subshell(lexer);
    }
    else
    {
        return NULL;
    }
}

// This function is here to handle all the prefix in the
// simple_command function
// it return 0 if it didn't find any prefix
// and 1 if it find 1 or more prefix

static int eat_prefix(struct lexer **lexer, struct AST *ast)
{
    int pref = 0;

    while (get_current_token(*lexer)
           && first_prefix(
               get_current_token(*lexer))) // va manger tous les prefix du debut
    {
        pref = 1;

        struct AST *child = prefix(lexer);

        if (child == NULL) // remonte l'erreur
            return -1;

        ast = add_children(ast, child);
    }

    return pref;
}

// this function's goal is respect the rule 2 of simple_command
// in fact, it eat the first Word or subshell
// and after it eat all the element
// if there is an issue it return -1
// else it return 0

static int eat_word_and_element(struct lexer **lexer, struct AST *ast)
{
    struct AST *child;

    if (!is_valid_word(*lexer))
    { // pas un mot alors c une erreur
        return -1;
    }

    child = create_ast(
        AST_VALUE,
        strdup(get_current_content(*lexer))); // prend la valeur du WORDS

    ast = add_children(ast, child);

    // pour gerer les subshell

    if (get_current_type(*lexer) == SUBSHELL && !verif_subshell(*lexer))
    {
        return -1;
    }

    *lexer = eat(*lexer);

    while (get_current_token(*lexer)
           && (first_element(get_current_token(*lexer))
               || is_valid_word(*lexer))) // first de element ou un valid word
                                          // car element peut etre just word
    {
        struct AST *child_el = element(lexer);
        if (child_el == NULL) // remonte l'err
            return -1;

        ast = add_children(ast, child_el);
    }

    return 0;
}

//(18) simple_command =
//         prefix {prefix}
//(19)    | {prefix} ( WORD | SUBSHELL ) { element }

struct AST *simple_command(struct lexer **lexer)
{
    struct AST *ast = create_ast(AST_SIMPLE_CMD, NULL);

    int pref = eat_prefix(lexer, ast);

    if (pref == -1)
    {
        goto err;
    }

    if (pref == 0
        && get_current_token(*lexer) == NULL) // pas de prefix et rien apres
    {
        goto err;
    }

    if (pref == 0
        || (get_current_token(*lexer) && is_valid_word(*lexer))) // regle 2
    {
        if (eat_word_and_element(lexer, ast) == -1)
        { // mange le word | subshell puis {element}
            goto err;
        }
    }

    if (!follow_simple_command(get_current_token(
            *lexer))) // si ce qui suit n'est pas bon direct erreur
    {
        goto err;
    }

    return ast;

err:
    destroy_AST(ast);
    return NULL;
}

// funcdec = WORD '(' ')' {'\n'} shell_command ;

struct AST *funcdec(struct lexer **lexer)
{
    if (get_current_token(*lexer) == NULL || get_current_type(*lexer) != WORDS)
    { // pas un bon mot
        return NULL;
    }

    *lexer = eat(*lexer);

    if (get_current_token(*lexer) == NULL
        || get_current_type(*lexer) != L_PAREN)
    { // pas paren
        return NULL;
    }

    *lexer = eat(*lexer);

    if (get_current_token(*lexer) == NULL
        || get_current_type(*lexer) != R_PAREN)
    { // pas paren
        return NULL;
    }

    *lexer = eat(*lexer);

    eat_newlines(lexer);

    return shell_command(lexer);
}
