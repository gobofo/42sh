#include "parser.h"

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
    struct AST *ast = create_ast(INPUT, NULL);

    if ((*token)->type == NEWLINE) // \n uniquement
    {
        *token = eat((*token));
        if ((*token) != NULL) // verifie que c est fini
        {
            goto err;
        }
        return ast;
    }

    else if ((*token)->type == WORDS
             || (*token)->type == IF) // verifie : list \n ou list EOF
    {
        struct AST *child = list(token);
        if (child == NULL)
        {
            destroy_AST(ast);
            return NULL;
        }
        ast = add_children(ast, child);

        if ((*token) != NULL && (*token)->type != NEWLINE)
        {
            goto err;
        }
        if ((*token)->type == NEWLINE)
        {
            *token = eat(*token);
            if (*token != NULL)
            {
                goto err;
            }
        }
    }
    return ast;

err:
    free_token(*token);
    destroy_AST(ast);
    return NULL;
}

//(3) list = and_or { ';' and_or } [ ';' ]
struct AST *list(struct token **token)
{
    struct AST *ast = create_ast(LIST, NULL);

    if ((*token)->type == WORDS || (*token)->type == IF)
    {
        struct AST *child = and_or(token);
        if (child == NULL)
        {
            destroy_AST(ast);
            return NULL;
        }
        ast = add_children(ast, child);
        if ((*token)->type == SEMICOLON)
        {
            *token = eat(*token);
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
            if ((*token)->type == SEMICOLON)
            {
                *token = eat(*token);
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        free_token(*token);
        destroy_AST(ast);
        return NULL;
    }
    return ast;
}

//(4) and_or = pipeline

struct AST *and_or(struct token **token)
{
    struct AST *ast = create_ast(AND_OR, NULL);

    if ((*token)->type == WORDS || (*token)->type == IF)
    {
        struct AST *child = pipeline(token);
        if (child == NULL)
        {
            destroy_AST(ast);
            return NULL;
        }
        ast = add_children(ast, child);
    }
    else
    {
        free_token(*token);
        destroy_AST(ast);
        return NULL;
    }
    return ast;
}

//(5) pipeline = command

struct AST *pipeline(struct token **token)
{
    struct AST *ast = create_ast(PIPELINE, NULL);

    if ((*token)->type == WORDS || (*token)->type == IF)
    {
        struct AST *child = command(token);
        if (child == NULL)
        {
            destroy_AST(ast);
            return NULL;
        }
        ast = add_children(ast, child);
    }
    else
    {
        free_token(*token);
        destroy_AST(ast);
        return NULL;
    }
    return ast;
}
// command =
//(6)    simple_command
//(7)    | shell_command
// entation de la fonction libre indique explicitement qu'il est possible de lui
// passer un pointeur n

struct AST *command(struct token **token)
{
    struct AST *ast = create_ast(COMMAND, NULL);

    if ((*token)->type == WORDS)
    {
        struct AST *child = simple_command(token);
        if (child == NULL)
        {
            destroy_AST(ast);
            return NULL;
        }
        ast = add_children(ast, child);
    }
    else if ((*token)->type == IF)
    {
        struct AST *child = shell_command(token);
        if (child == NULL)
        {
            destroy_AST(ast);
            return NULL;
        }
        ast = add_children(ast, child);
    }
    else
    {
        free_token(*token);
        destroy_AST(ast);
        return NULL;
    }
    return ast;
}

//(8) shell_command = rule_if

struct AST *shell_command(struct token **token)
{
    struct AST *ast = create_ast(SHELL_COMMAND, NULL);
    if ((*token)->type == IF)
    {
        struct AST *child = rule_if(token);
        if (child == NULL)
        {
            destroy_AST(ast);
            return NULL;
        }
        ast = add_children(ast, child);
    }
    else
    {
        free_token(*token);
        destroy_AST(ast);
        return NULL;
    }
    return ast;
}
//(9) rule_if = 'if' compound_list 'then' compound_list [else_clause] 'fi'

struct AST *rule_if(struct token **token)
{
    struct AST *ast = create_ast(RULE_IF, NULL);
    if ((*token)->type == IF)
    { // regle 9
        *token = eat(*token);

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

            if ((*token)->type != THEN)
            { // verifie la GRAMMAR
                goto err;
            }
            *token = eat(*token);

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
            if ((*token)->type != FI)
            { // verifie la GRAMMAR FIN
                goto err;
            }
            *token = eat(*token);
        }
        else
        { // pas de premier counpound list
            free_token(*token);
            destroy_AST(ast);
            return NULL;
        }
    }

    else
    { // pas de premier IF donc pas de regle trouvee
        goto err;
    }

    return ast;

err:
    free_token(*token);
    destroy_AST(ast);
    return NULL;
}

// else_clause =
//(10)    'else' compound_list
//(11)    | 'elif' compound_list 'then' compound_list [else_clause]

struct AST *else_clause(struct token **token)
{
    struct AST *ast = create_ast(ELSE_CLAUSE, NULL);
    if ((*token)->type == ELSE)
    {
        *token = eat(*token);

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
    }
    if ((*token)->type == ELIF) // UN COPIER COLLER DE IF
    { // regle 9
        *token = eat(*token);

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

            if ((*token)->type != THEN)
            { // verifie la GRAMMAR
                goto err;
            }
            *token = eat(*token);

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
            if ((*token)->type != FI)
            { // verifie la GRAMMAR FIN
                goto err;
            }
            *token = eat(*token);
        }
        else
        { // pas de premier counpound list
            free_token(*token);
            destroy_AST(ast);
            return NULL;
        }
    }

    else
    { // pas de regle TROUVEE
        goto err;
    }

    return ast;

err:
    free_token(*token);
    destroy_AST(ast);
    return NULL;
}

//(12) compound_list = {'\n'} and_or { ( ';' | '\n' ) {'\n'} and_or } [';']
//{'\n'}

struct AST *compound_list(struct token **token)
{
    struct AST *ast = create_ast(COMPOUND_LIST, NULL);

    while ((*token)->type == NEWLINE)
    {
        *token = eat(*token);
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
    while ((*token)->type == NEWLINE || (*token)->type == SEMICOLON)
    {
        if ((*token)->type == SEMICOLON)
        {
            *token = eat(*token);
            if ((*token)->type == THEN || (*token)->type == ELSE
                || (*token)->type == ELIF || (*token)->type == FI)
            { // follow de compound_list donc on sort
                return ast;
            }
        }
        while ((*token)->type == NEWLINE)
        {
            *token = eat(*token);
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
    free_token(*token);
    destroy_AST(ast);
    return NULL;
}
//(13) simple_command = WORD { element }
//

struct AST *simple_command(struct token **token)
{
    struct AST *ast = create_ast(SIMPLE_COMMAND, strdup((*token)->content));

    if ((*token)->type == WORDS)
    {
        *token = eat(*token);
        while ((*token)->type == WORDS)
        {
            struct AST *child = element(token);
            if (child == NULL)
            {
                destroy_AST(ast);
                return NULL;
            }
            ast = add_children(ast, child);
        }
    }
    else
    {
        free_token(*token);
        destroy_AST(ast);
        return NULL;
    }
    return ast;
}

//(14) element = WORD
//

struct AST *element(struct token **token)
{
    struct AST *ast = create_ast(SIMPLE_COMMAND, strdup((*token)->content));
    if ((*token)->type == WORDS)
    {
        return ast;
    }
    else
    {
        free_token(*token);
        destroy_AST(ast);
        return NULL;
    }
}
