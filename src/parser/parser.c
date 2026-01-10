#include "parser.h"

#include <string.h>

#include "../ast/ast.h"
#include "../lexer/lexer.h"
#include "../token.h"

// #include "../lexer/lexer.h"

/* === GRAMMAR === */

/*

    input =
(1)     list '\n'
(2)    | '\n'

(3) list = and_or { ';' and_or } [ ';' ]

(4) and_or = pipeline

(5) pipeline = command

    command =
(6)    simple_command
(7)    | shell_command

(8) shell_command = rule_if

(9) rule_if = 'if' compound_list 'then' compound_list [else_clause] 'fi'

    else_clause =
(10)    'else' compound_list
(11)    | 'elif' compound_list 'then' compound_list [else_clause]

(12) compound_list = {'\n'} and_or { ( ';' | '\n' ) {'\n'} and_or } [';'] {'\n'}

(13) simple_command = WORD { element }

(14) element = WORD

*/

struct AST *input(struct token **token);
struct AST *list(struct token **token);
struct AST *and_or(struct token **token);
struct AST *pipeline(struct token **token);
struct AST *command(struct token **token);
struct AST *shell_command(struct token **token);
struct AST *rule_if(struct token **token);
struct AST *else_clause(struct token **token);
struct AST *compound_list(struct token **token);
struct AST *simple_command(struct token **token);
struct AST *element(struct token **token);

struct token *eat(struct token *token)
{
    free_token(token);
    return get_token(NULL);
}

//    input =
//(1)     list '\n'
//(2)    | '\n'
struct AST *input(struct token **token)
{
    if ((*token)->type == NEWLINE)
    {
        free_token(*token);
        return create_ast(AST_LIST, NULL);
    }

    else if ((*token)->type == WORDS || (*token)->type == IF)
    {
        struct AST *ast = list(token);
        if (ast == NULL)
        {
            free_token(*token);
            return NULL;
        }

        if ((*token) != NULL && (*token)->type != NEWLINE)
        {
            free_token(*token);
            destroy_AST(ast);
            return NULL;
        }
        if (*token != NULL)
        {
            free_token(*token);
        }
        return ast;
    }
    else
    {
        free_token(*token);
        return NULL;
    }
}

//(3) list = and_or { ';' and_or } [ ';' ]
struct AST *list(struct token **token)
{
    struct AST *ast = create_ast(AST_LIST, NULL);

    if ((*token)->type == WORDS || (*token)->type == IF)
    {
        struct AST *child = and_or(token);
        if (child == NULL)
        {
            destroy_AST(ast);
            return NULL;
        }
        ast = add_children(ast, child);

        if (*token == NULL)
        { // AJOUTER POUR GERER LES NULL

            return ast;
        }

        if ((*token)->type == SEMICOLON)
        {
            *token = eat(*token);

            if (*token == NULL || (*token)->type == NEWLINE)
            { // AJOUTER POUR GERER LES NULL
                return ast;
            }
        }
        else
        {
            return ast;
        }
        while ((*token)->type == WORDS || (*token)->type == IF)
        {
            struct AST *child = and_or(token);
            if (child == NULL)
            {
                destroy_AST(ast);
                return NULL;
            }
            ast = add_children(ast, child);

            if (*token == NULL)
            { // AJOUTER POUR GERER LES NULL

                return ast;
            }

            if ((*token)->type == SEMICOLON)
            {
                *token = eat(*token);

                if (*token == NULL)
                {
                    //destroy_AST(ast);
                    return ast;
                }
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        destroy_AST(ast);
        return NULL;
    }
    return ast;
}

//(4) and_or = pipeline

struct AST *and_or(struct token **token)
{
    if ((*token)->type == WORDS || (*token)->type == IF)
    {
        struct AST *ast = pipeline(token);
        return ast;
    }
    else
    {
        return NULL;
    }
}

//(5) pipeline = command

struct AST *pipeline(struct token **token)
{
    if ((*token)->type == WORDS || (*token)->type == IF)
    {
        struct AST *ast = command(token);
        return ast;
    }
    else
    {
        return NULL;
    }
}

// command =
//(6)    simple_command
//(7)    | shell_command
// entation de la fonction libre indique explicitement qu'il est possible de lui
// passer un pointeur n

struct AST *command(struct token **token)
{
    if ((*token)->type == WORDS)
    {
        struct AST *ast = simple_command(token);
        return ast;
    }
    else if ((*token)->type == IF)
    {
        struct AST *ast = shell_command(token);
        return ast;
    }
    else
    {
        return NULL;
    }
}

//(8) shell_command = rule_if

struct AST *shell_command(struct token **token)
{
    if ((*token)->type == IF)
    {
        struct AST *ast = rule_if(token);
        return ast;
    }
    else
    {
        return NULL;
    }
}
//(9) rule_if = 'if' compound_list 'then' compound_list [else_clause] 'fi'

struct AST *rule_if(struct token **token)
{
    struct AST *ast = create_ast(AST_IF, NULL);
    if ((*token)->type == IF)
    { // regle 9
        *token = eat(*token);

        if (*token == NULL)
        { // AJOUTER POUR GERER LES NULL

            goto err;
        }

        if ((*token)->type == NEWLINE || (*token)->type == IF
            || (*token)->type == WORDS)
        { // first de compound_list

            struct AST *child = compound_list(token);
            if (child == NULL)
            {
                destroy_AST(ast);
                return NULL;
            }
            ast = add_children(ast, child);

            if (*token == NULL
                || (*token)->type != THEN) // AJOUTER POUR GERER LES NULL
            { // verifie la GRAMMAR
                goto err;
            }
            *token = eat(*token);

            if (*token == NULL)
            { // AJOUTER POUR GERER LES NULL

                goto err;
            }

            if ((*token)->type == NEWLINE || (*token)->type == IF
                || (*token)->type == WORDS)
            { // first de compound_list

                struct AST *child = compound_list(token);
                if (child == NULL)
                {
                    destroy_AST(ast);
                    return NULL;
                }
                ast = add_children(ast, child);
            }
            else // pas de deuxime counpound list
            {
                goto err;
            }

            if (*token == NULL)
            { // AJOUTER POUR GERER LES NULL

                goto err;
            }

            if ((*token)->type == ELIF || (*token)->type == ELSE)
            { // esle clause existe
                struct AST *child = else_clause(token);
                if (child == NULL)
                {
                    destroy_AST(ast);
                    return NULL;
                }
                ast = add_children(ast, child);
            }
            if ((*token)->type != FI)
            { // verifie la GRAMMAR FIN
                goto err;
            }
            *token = eat(*token);
        }
        else
        { // pas de premier counpound list
            goto err;
        }
    }

    else
    { // pas de premier IF donc pas de regle trouvee
        goto err;
    }

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

    if ((*token)->type == ELSE)
    {
        *token = eat(*token);

        if (*token == NULL)
        { // AJOUTER POUR GERER LES NULL

            return NULL;
        }

        ast = compound_list(token);
        return ast;
    }
    else if ((*token)->type == ELIF)
    {
        ast = create_ast(AST_IF, NULL);
        *token = eat(*token);

        if (*token == NULL)
        { // AJOUTER POUR GERER LES NULL

            goto err;
        }

        if ((*token)->type == NEWLINE || (*token)->type == IF
            || (*token)->type == WORDS)
        { // first de compound_list
            struct AST *child = compound_list(token);

            if (child == NULL)
            {
                destroy_AST(ast);
                return NULL;
            }

            ast = add_children(ast, child);

            if (*token == NULL
                || (*token)->type != THEN) // AJOUTER POUR GERER LES NULL
            { // verifie la GRAMMAR
                goto err;
            }
            *token = eat(*token);

            if (*token == NULL)
            { // AJOUTER POUR GERER LES NULL

                goto err;
            }

            if ((*token)->type == NEWLINE || (*token)->type == IF
                || (*token)->type == WORDS)
            { // first de compound_list
                struct AST *child = compound_list(token);
                if (child == NULL)
                {
                    destroy_AST(ast);
                    return NULL;
                }
                ast = add_children(ast, child);
            }
            else // pas de deuxime counpound list
            {
                goto err;
            }
            if ((*token)->type == ELIF || (*token)->type == ELSE)
            { // esle clause existe
                struct AST *child = else_clause(token);
                if (child == NULL)
                {
                    destroy_AST(ast);
                    return NULL;
                }
                ast = add_children(ast, child);
            }
            return ast;
        }
        else
        { // pas de premier counpound list
            goto err;
        }
    }

    else
    { // pas de premier IF donc pas de regle trouvee
        goto err;
    }

err:
    destroy_AST(ast);
    return NULL;
}

//(12) compound_list = {'\n'} and_or { ( ';' | '\n' ) {'\n'} and_or } [';']
//{'\n'}

struct AST *compound_list(struct token **token)
{
    struct AST *ast = create_ast(AST_LIST, NULL);

    while ((*token)->type == NEWLINE)
    {
        *token = eat(*token);

        if (*token == NULL)
        { // AJOUTER POUR GERER LES NULL
            goto err;
        }
    }
    if ((*token)->type == WORDS || (*token)->type == IF)
    { // and_or
        struct AST *child = and_or(token);
        if (child == NULL)
        {
            destroy_AST(ast);
            return NULL;
        }
        ast = add_children(ast, child);
    }
    else
    {
        goto err;
    }

    if (*token == NULL)
    { // AJOUTER POUR GERER LES NULL

        goto err;
    }

    while ((*token)->type == NEWLINE || (*token)->type == SEMICOLON)
    {
        if ((*token)->type == SEMICOLON)
        {
            *token = eat(*token);

            if (*token == NULL)
            { // AJOUTER POUR GERER LES NULL

                goto err;
            }

            if ((*token)->type == THEN || (*token)->type == ELSE
                || (*token)->type == ELIF || (*token)->type == FI)
            { // follow de compound_list donc on sort
                return ast;
            }
        }
        while ((*token)->type == NEWLINE)
        {
            *token = eat(*token);

            if (*token == NULL)
            { // AJOUTER POUR GERER LES NULL

                goto err;
            }
        }
        if ((*token)->type == WORDS || (*token)->type == IF)
        {
            struct AST *child = and_or(token);
            if (child == NULL)
            {
                destroy_AST(ast);
                return NULL;
            }
            ast = add_children(ast, child);
        }
    }
    return ast;

err:
    destroy_AST(ast);
    return NULL;
}
//(13) simple_command = WORD { element }
//

struct AST *simple_command(struct token **token)
{
    struct AST *ast = create_ast(AST_CMD, NULL);

    if ((*token)->type == WORDS)
    {
        struct AST *ast_c = create_ast(AST_VALUE, strdup((*token)->content));
        ast = add_children(ast, ast_c);

        *token = eat(*token);

        if (*token == NULL)
        { // AJOUTER POUR GERER LES NULL

            return ast;
        }

        while ((*token)->type == WORDS)
        {
            struct AST *child = element(token);
            if (child == NULL)
            {
                destroy_AST(ast);
                return NULL;
            }
            ast = add_children(ast, child);

            if (*token == NULL || (*token)->type == NEWLINE)
            { // AJOUTER POUR GERER LES NULL

                return ast;
            }
        }
    }
    else
    {
        destroy_AST(ast);
        return NULL;
    }
    return ast;
}

//(14) element = WORD
//

struct AST *element(struct token **token)
{
    struct AST *ast = create_ast(AST_VALUE, strdup((*token)->content));
    if ((*token)->type == WORDS)
    {
        *token = eat(*token);
        return ast;
    }
    else
    {
        destroy_AST(ast);
        return NULL;
    }
}
