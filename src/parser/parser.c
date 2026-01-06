#include "parser.h"

#include <stdio.h>

#include "../ast/ast.h"
//#include "../lexer/lexer.h"

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

struct AST *input(struct node *node);
struct AST *list(struct node *node);
struct AST *and_or(struct node *node);
struct AST *pipeline(struct node *node);
struct AST *command(struct node *node);
struct AST *shell_command(struct node *node);
struct AST *rule_if(struct node *node);
struct AST *else_clause(struct node *node);
struct AST *compound_list(struct node *node);
struct AST *simple_command(struct node *node);
struct AST *element(struct node *node);

struct AST *input(struct node *node){

    struct AST *input = create_ast(INPUT, NULL);

    if (node != NULL && node->token->type != NEWLINE ) { //cas avec une list

        struct AST *list = list(node);
        input = add_children(input, list);

    }

    return input;

}

struct AST *list(struct node *node){

    struct AST *list = create_ast(LIST, NULL);

    

}
