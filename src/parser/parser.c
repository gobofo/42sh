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

int parse_body_if(struct AST** ast,struct token **token);

static bool is_valid_word(struct token *token)
{
    if (token == NULL)
    {
        return false;
    }

    enum types type = token->type;

    return type != NEWLINE && type != AND && type != OR && type != SEMICOLON
        && type != PIPE && type != REDIR;
}

static struct token *eat(struct token *token)
{
    free_token(token);
    return get_token(NULL);
}

static void eat_newlines(struct token **token)
{
    while (*token && (*token)->type == NEWLINE)
        *token = eat(*token);
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
} //17 ligne 

//(3) list = and_or { 'w' and_or } [ ';' ]
static struct AST *list(struct token **token)
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
//32 ligne 
//

//(6) and_or = pipeline { ( '&&' | '||' ) {'\n'} pipeline }


static struct AST *and_or(struct token **token)
{
    struct AST *ast = NULL;
    if (first_pipeline(*token))
    {
        ast = pipeline(token);
        if (ast == NULL)
        {
            goto err;
        }
        while (*token && ((*token)->type == AND || (*token)->type == OR))
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
//39

static struct AST *pipeline(struct token **token)
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
//32
//

// command =
//(6)    simple_command
//(7)    | shell_command
// entation de la fonction libre indique explicitement qu'il est possible de
// lui passer un pointeur n

static struct AST *command(struct token **token)
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
}//22

//(8) shell_command = rule_if

static struct AST *shell_command(struct token **token)
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
}//22

//(9) rule_if = 'if' compound_list 'then' compound_list [else_clause] 'fi'

static struct AST *rule_if(struct token **token)
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

            if (first_else_clause(*token))
            { // esle clause existe
                struct AST *child = else_clause(token);
                if (child == NULL)
                {
                    destroy_AST(ast);
                    return NULL;
                }
                ast = add_children(ast, child);
            }

            if (*token == NULL || (*token)->type != FI)
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
} //37 

// else_clause =
//(10)    'else' compound_list
//(11)    | 'elif' compound_list 'then' compound_list [else_clause]

static struct AST *else_clause(struct token **token)
{
    struct AST *ast = NULL;

    if ((*token)->type == ELSE)
    {
        *token = eat(*token);

        if (!first_compound_list(*token))
        {
            return NULL;
        }
        ast = compound_list(token);
        return ast;
    }
    else if ((*token)->type == ELIF)
    {
        ast = create_ast(AST_IF, NULL);
        *token = eat(*token);

        if (!first_compound_list(*token))
        {
            goto err;
        }

        struct AST *child = compound_list(token);
        if (child == NULL)
        {
            goto err;
        }
        ast = add_children(ast, child);

        if (*token == NULL || (*token)->type != THEN)
        {
            goto err;
        }
        *token = eat(*token);

        if (first_compound_list(*token))
        {
            struct AST *child = compound_list(token);
            if (child == NULL)
            {
                goto err;
            }
            ast = add_children(ast, child);
        }
        else // pas de deuxime counpound list
        {
            goto err;
        }
        if (first_else_clause(*token))
        { // esle clause existe
            struct AST *child = else_clause(token);
            if (child == NULL)
            {
                goto err;
            }
            ast = add_children(ast, child);
        }
        return ast;
    }
    else
    { // pas de elif ou else
        goto err;
    }

err:
    destroy_AST(ast);
    return NULL;
} //37

//(14) rule_while = 'while' compound_list 'do' compound_list 'done'
static struct AST *rule_while(struct token **token)
{
    struct AST *ast = create_ast(AST_WHILE, NULL);
    if ((*token)->type == WHILE)
    {
        *token = eat(*token);

        if (first_compound_list(*token))
        {
            struct AST *child = compound_list(token);

            if (child == NULL)
                goto err;

            ast = add_children(ast, child);

            if (*token == NULL || (*token)->type != DO) // pas de DO
                goto err;

            *token = eat(*token);

            if (first_compound_list(*token))
            {
                struct AST *child = compound_list(token);

                if (child == NULL)
                    goto err;

                ast = add_children(ast, child);

                if (*token == NULL || (*token)->type != DONE) // pas de done
                    goto err;
            }
            else
            { // pas de deuxime compound_list
                goto err;
            }

            *token = eat(*token);

            return ast;
        }
    }
err:
    destroy_AST(ast);
    return NULL;
}//25

//(15) rule_until = 'until' compound_list 'do' compound_list 'done'
static struct AST *rule_until(struct token **token)
{
    struct AST *ast = create_ast(AST_UNTIL, NULL);
    if ((*token)->type == UNTIL)
    {
        *token = eat(*token);

        if (first_compound_list(*token))
        {
            struct AST *child = compound_list(token);

            if (child == NULL)
                goto err;

            ast = add_children(ast, child);

            if (*token == NULL || (*token)->type != DO) // pas de DO
                goto err;

            *token = eat(*token);

            if (first_compound_list(*token))
            {
                struct AST *child = compound_list(token);

                if (child == NULL)
                    goto err;

                ast = add_children(ast, child);

                if (*token == NULL || (*token)->type != DONE) // pas de done
                    goto err;
            }
            else
            { // pas de deuxime compound_list
                goto err;
            }

            *token = eat(*token);

            return ast;
        }
    }
err:
    destroy_AST(ast);
    return NULL;
}//27

//(16) rule_for = 'for' WORD ( [';']
//                          | [ {'\n'} 'in' { WORD } ( ';' | '\n' ) ] )
//{'\n'} 'do' compound_list 'done'

static struct AST *rule_for(struct token **token)
{
    struct AST *ast = create_ast(AST_FOR, NULL);
    if ((*token)->type == FOR)
    {
        *token = eat(*token);
        if (*token == NULL)
        {
            goto err;
        }

        if (is_valid_word(*token))
        {
            struct AST *child =
                create_ast(AST_VALUE, strdup((*token)->content)); // WORDS
            ast = add_children(ast, child);
            *token=eat(*token);
        }

        else
        {
            goto err;
        }

        if (*token == NULL)
            goto err;

        if ((*token)->type == SEMICOLON) // cas 1
            *token = eat(*token);

        else if ((*token)->type == NEWLINE || (*token)->type == IN)
        { // cas 2
            eat_newlines(token);
            if (*token == NULL || (*token)->type != IN)
                goto err;

            *token = eat(*token);
            while (*token != NULL && (*token)->type != SEMICOLON
                   && (*token)->type != NEWLINE)
            {
                if (is_valid_word(*token))
                {
                    struct AST *child = create_ast(
                        AST_VALUE, strdup((*token)->content)); // WORDS
                    ast = add_children(ast, child);
                    *token = eat(*token);
                }

                else
                {
                    goto err;
                }
            }
            if (*token && ((*token)->type == NEWLINE || (*token)->type == SEMICOLON))
            { // fin
                *token = eat(*token);
            }
            else
            {
                goto err;
            }
        }
        else{
          goto err;
        }
        eat_newlines(token);

        if(parse_body_if(&ast,token)){
          return ast;
        }
    }
err:
    destroy_AST(ast);
    return NULL;
} //==40

int parse_body_if(struct AST** ast,struct token **token){
  if (*token == NULL || (*token)->type != DO)
    return 0;
  *token = eat(*token);

  if (first_compound_list(*token))
  {
    struct AST *child = compound_list(token);

    if (child == NULL)
      return 0;

    *ast = add_children(*ast, child);

    if (*token == NULL || (*token)->type != DONE)
      return 0;
    *token = eat(*token);
    return 1;
  }
  return 0;

}


//(12) compound_list = {'\n'} and_or { ( ';' | '\n' ) {'\n'} and_or } [';']
//{'\n'}

static struct AST *compound_list(struct token **token)
{
    struct AST *ast = create_ast(AST_LIST, NULL);

    while (*token != NULL && (*token)->type == NEWLINE)
    {
        *token = eat(*token);
    }
    if (first_and_or(*token))
    {
        struct AST *child = and_or(token);
        if (child == NULL)
            goto err;

        ast = add_children(ast, child);
    }
    else
    {
        goto err;
    }
    if(*token==NULL) 
      goto err;

    while ((*token)->type == NEWLINE || (*token)->type == SEMICOLON)
    {
        if ((*token)->type == SEMICOLON)
        {
            *token = eat(*token);

            if (*token == NULL)
                goto err;

            if (follow_compound_list(*token))
            { // follow de compound_list donc on sort
                return ast;
            }
        }
        while ((*token)->type == NEWLINE)
        {
            *token = eat(*token);

            if (*token == NULL)
            {
                goto err;
            }
        }
        if (first_and_or(*token))
        {
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
    }

	eat_newlines(token);

    return ast;

err:
    destroy_AST(ast);
    return NULL;
} //36

//(18) simple_command =
//         prefix {prefix}
//(19)    | {prefix} WORD { element }

static struct AST *simple_command(struct token **token)
{
    struct AST *ast = create_ast(AST_SIMPLE_CMD, NULL);

    int pref = 0;

    while (*token && first_prefix(*token) )
    {
        pref = 1;
        struct AST *child = prefix(token);
        if (child == NULL)
            goto err;
        ast = add_children(ast, child);
    }

    if (pref == 0 && *token == NULL)
    {
        goto err;
    }

    if (pref == 0 || (*token && is_valid_word(*token))) // regle 2
    {
        struct AST* child;
        if(is_valid_word(*token)){
            child = create_ast(AST_VALUE, strdup((*token)->content));
        }
        else{
          goto err;
        }

        ast = add_children(ast, child);

        *token = eat(*token);

        while (*token && (first_element(*token) || is_valid_word(*token)))
        {
            struct AST *child_el = element(token);
            if (child_el == NULL)
                goto err;

            ast = add_children(ast, child_el);
        }

        if (follow_simple_command(*token))
        {
            return ast;
        }
        else
        {
            goto err;
        }
    }
    else
    {
        if (follow_simple_command(*token))
        {
            return ast;
        }

        else
        {
            goto err;
        }
    }
err:
    destroy_AST(ast);
    return NULL;
} //>34

//(20) prefix =
//        redirection
//        | ASSIGNEMENT_WORD

static struct AST *prefix(struct token **token)
{
    if ((*token)->type == A_WORDS)
    {
        struct AST *ast = create_ast(AST_ASSIGNEMENT, strdup((*token)->content));
		*token = eat(*token);
        return ast;
    }

    else if (first_redirection(*token))
    {
        struct AST *ast = redirection(token);
        if (ast == NULL)
        {
            return NULL;
        }
        return ast;
    }

    return NULL;
}

//(21) redirection = [IONUMBER] ( '>' | '<' | '>>' | '>&' | '<&' | '>|' | '<>' )
//WORD

static struct AST *redirection(struct token **token)
{
    struct AST *ast = NULL;

    if ((*token)->type == REDIR)
    {
        ast = create_ast(AST_REDIR, NULL);
        struct AST *ast_val = create_ast(AST_VALUE, strdup((*token)->content));
        ast = add_children(ast, ast_val);

        *token = eat(*token);

        if (is_valid_word(*token))
        {
            struct AST *ast_val2 =
                create_ast(AST_VALUE, strdup((*token)->content));
            ast = add_children(ast, ast_val2);
            *token = eat(*token);
            return ast;
        }

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

//(22) element =
//        WORD
//        | redirection

static struct AST *element(struct token **token)
{
    if (is_valid_word(*token))
    {
        struct AST *ast = create_ast(AST_VALUE, strdup((*token)->content));
        *token = eat(*token);
        return ast;
    }

    else if ((*token)->type == REDIR)
    {
        struct AST *ast = redirection(token);
        if (ast == NULL)
        {
            return NULL;
        }
        return ast;
    }

    else
    {
        return NULL;
    }
}
