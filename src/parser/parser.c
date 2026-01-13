#include "parser.h"

#include <stdbool.h>
#include <string.h>

#include "../ast/ast.h"
#include "../lexer/lexer.h"
#include "../token.h"
// #include "../lexer/lexer.h"

/* === GRAMMAR === */

/*

    input =
(1)     list '\n'
(2)    | list EOF
(3)    | '\n'
(4)    | EOF

(5) list = and_or { ';' and_or } [ ';' ]

(6) and_or = pipeline { ( '&&' | '||' ) {'\n'} pipeline }

(7) pipeline = [!] command { '|' {'\n'} command }

    command =
(8)    simple_command
(9)    | shell_command {redirection}

(10) shell_command =
        rule_if
        | rule_while
        | rule_until
        | rule_for

(11) rule_if = 'if' compound_list 'then' compound_list [else_clause] 'fi'

    else_clause =
(12)    'else' compound_list
(13)    | 'elif' compound_list 'then' compound_list [else_clause]

(14) rule_while = 'while' compound_list 'do' compound_list 'done'

(15) rule_until = 'until' compound_list 'do' compound_list 'done'

(16) rule_for = 'for' WORD ( [';'] | [ {'\n'} 'in' { WORD } ( ';' | '\n' ) ] )
{'\n'} 'do' compound_list 'done'

(17) compound_list = {'\n'} and_or { ( ';' | '\n' ) {'\n'} and_or } [';'] {'\n'}

(18) simple_command =
          prefix {prefix}
(19)    | {prefix} WORD { element }

(20) prefix =
        redirection
        | ASSIGNEMENT_WORD

(21) redirection = [IONUMBER] ( '>' | '<' | '>>' | '>&' | '<&' | '>|' | '<>' )
WORD

(22) element =
        WORD
        | redirection
*/

struct AST *input(struct token **token);
static struct AST *list(struct token **token);
static struct AST *and_or(struct token **token);
static struct AST *pipeline(struct token **token);
static struct AST *command(struct token **token);
static struct AST *shell_command(struct token **token);
static struct AST *rule_if(struct token **token);
static struct AST *else_clause(struct token **token);
static struct AST *compound_list(struct token **token);
static struct AST *simple_command(struct token **token);
static struct AST *element(struct token **token);
static struct AST *redirection(struct token **token);
static struct AST *prefix(struct token **token);
static struct AST *rule_for(struct token **token);
static struct AST *rule_while(struct token **token);
static struct AST *rule_until(struct token **token);

struct token *eat(struct token *token)
{
    free_token(token);
    return get_token(NULL);
}

//    input =
//(1)     list '\n'
//(2)    | list EOF
//(3)    | '\n'
//(4)    | EOF

struct AST *input(struct token **token)
{
    if (*token == NULL || (*token)->type == NEWLINE)
    {
        free_token(*token);
        return create_ast(AST_LIST, NULL);
    }

    else if (first_list(*token))
    {
        struct AST *ast = list(token);
        if (ast == NULL)
        {
            free_token(*token);
            return NULL;
        }

        if (!follow_list(*token))
        {
            free_token(*token);
            destroy_AST(ast);
            return NULL;
        }
        free_token(*token); // si \NEWLINE
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

    if (first_and_or(*token))
    {
        struct AST *child = and_or(token);
        if (child == NULL)
        {
            goto err;
        }
        ast = add_children(ast, child);

        if (follow_list(*token))
        {
            return ast;
        }

        if ((*token)->type == SEMICOLON)
        {
            *token = eat(*token);

            if (follow_list(*token))
            {
                return ast;
            }
        }
        else
        {
            goto err;
        }
        while (first_and_or(*token))
        {
            struct AST *child = and_or(token);
            if (child == NULL)
            {
                goto err;
            }
            ast = add_children(ast, child);

            if (follow_list(*token))
            {
                return ast;
            }

            if ((*token)->type == SEMICOLON)
            {
                *token = eat(*token);
            }

            if (follow_list(*token))
            {
                return ast;
            }
        }
        goto err;
    }
    else
    {
        goto err;
    }

err:
    destroy_AST(ast);
    return NULL;
}

//(6) and_or = pipeline { ( '&&' | '||' ) {'\n'} pipeline }

struct AST *and_or(struct token **token)
{
    struct AST *ast;
    if (first_pipeline(*token))
    {
        ast = pipeline(token);
        if (ast == NULL)
        {
            goto err;
        }
        while ((*token)->type == AND || (*token)->type == OR)
        {
            struct AST *ast_op;
            if ((*token)->type == AND)
            {
                ast_op = create_ast(AST_AND, NULL);
            }
            else
            {
                ast_op = create_ast(AST_OR, NULL);
            }
            *token = eat(*token);
            if (*token == NULL) // && ou || mais probleme ensuite
            {
                destroy_AST(ast_op);
                goto err;
            }
            while ((*token)->type == NEWLINE)
            {
                *token = eat(*token);
                if (*token == NULL) // && ou || mais probleme ensuite
                {
                    destroy_AST(ast_op);
                    goto err;
                }
            }
            if (!first_pipeline(*token))
            {
                destroy_AST(ast_op);
                goto err;
            }
            ast_op = add_children(ast_op, ast);
            ast = pipeline(token);
            if (ast == NULL)
            {
                destroy_AST(ast_op);
                goto err;
            }
            ast_op = add_children(ast_op, ast);
            ast = ast_op;
        }
        if (follow_and_or(*token))
            return ast;
        else
        {
            goto err;
        }
    }
    else
    {
        goto err;
    }
err:
    destroy_AST(ast);
    return NULL;
}

struct AST *pipeline(struct token **token)
{
    struct AST *ast = create_ast(AST_PIPELINE, NULL);
    if ((*token)->type == NEG)
    {
        ast->is_neg = 1;
        *token = eat(*token);
        if (*token == NULL)
        {
            goto err;
        }
    }
    if (first_command(*token))
    {
        struct AST *children = command(token);
        if (children == NULL)
        {
            goto err;
        }
        ast = add_children(ast, children);
    }
    else
    {
        goto err;
    }
    if (follow_pipeline(*token))
        return ast;

    while ((*token)->type == PIPE)
    {
        *token = eat(*token);
        if (*token == NULL)
            goto err;

        while ((*token)->type == NEWLINE)
        {
            *token = eat(*token);
            if (*token == NULL)
            {
                goto err;
            }
        }
        if (!first_command(*token))
            goto err;
        struct AST *children = command(token);
        if (children == NULL)
        {
            goto err;
        }
        ast = add_children(ast, children);
        if (follow_pipeline(*token))
            return ast;
    }
err:
    destroy_AST(ast);
    return NULL;
}

// command =
//(6)    simple_command
//(7)    | shell_command
// entation de la fonction libre indique explicitement qu'il est possible de
// lui passer un pointeur n

struct AST *command(struct token **token)
{
    if (first_simple_command(*token))
    {
        struct AST *ast = simple_command(token);
        return ast;
    }
    else if (first_shell_command(*token))
    {
        struct AST *ast = create_ast(AST_SHELL_CMD, NULL);
        struct AST *ast_shell = shell_command(token);
        if (ast_shell == NULL)
        {
            destroy_AST(ast);
            return NULL;
        }
        ast = add_children(ast, ast_shell);
        while (first_redirection(*token))
        {
            struct AST *ast_redir = redirection(token);
            if (ast_redir == NULL)
            {
                destroy_AST(ast);
                return NULL;
            }
            ast = add_children(ast, ast_redir);
        }
        if (follow_command(*token))
            return ast;

        destroy_AST(ast);
        return NULL;
    }
    else
    {
        return NULL;
    }
}

//(8) shell_command = rule_if

struct AST *shell_command(struct token **token)
{
    if (first_rule_if(*token))
    {
        struct AST *ast = rule_if(token);
        if (ast == NULL)
            return NULL;
        return ast;
    }
    else if (first_rule_while(*token))
    {
        struct AST *ast = rule_while(token);
        if (ast == NULL)
            return NULL;
        return ast;
    }
    else if (first_rule_until(*token))
    {
        struct AST *ast = rule_until(token);
        if (ast == NULL)
            return NULL;
        return ast;
    }
    else if (first_rule_for(*token))
    {
        struct AST *ast = rule_for(token);
        if (ast == NULL)
            return NULL;
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

        if (first_compound_list(*token))
        { // first de compound_list

            struct AST *child = compound_list(token);
            if (child == NULL)
            {
                return NULL;
            }
            ast = add_children(ast, child);

            if (*token == NULL || (*token)->type != THEN)
            {
                goto err;
            }
            *token = eat(*token);


            if (first_compound_list(*token))
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

            if (first_else_close(*token))
            { // esle clause existe
                struct AST *child = else_clause(token);
                if (child == NULL)
                {
                    destroy_AST(ast);
                    return NULL;
                }
                ast = add_children(ast, child);

            }

            if (*token ==NULL || (*token)->type != FI)
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
    struct AST *ast = create_ast(AST_SIMPLE_CMD, NULL);

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
