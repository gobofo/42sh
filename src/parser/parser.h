#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>

#include "../ast/ast.h"
#include "../lexer/lexer.h"
#include "../token.h"

/* ============= PRINT AST ============= */

#include "print_parser.h"

/* ============= USEFULL FONCTION ============= */

#include "usefull_fnct.h"

/* ============= FIRST ============= */

#include "parser_firsts/first_command.h"
#include "parser_firsts/first_condition.h"
#include "parser_firsts/first_loop.h"
#include "parser_firsts/first_sequence.h"
#include "parser_firsts/first_token.h"

/* ============= FOLLOW ============= */

#include "parser_follow.h"

/* ============= GRAMMAR ============= */

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
       |funcdec { redirection }


(10) shell_c
    '{' compound_list '}'
   |'(' compound_list ')'
   | rule_if
   | rule_while
   | rule_until
   | rule_for

funcdec = WORD '(' ')' {'\n'} shell_command ;

(11) rule_if = 'if' compound_list 'then' compound_list [else_clause] 'fi'

    else_clause =
(12)    'else' compound_list
(13)    | 'elif' compound_list 'then' compound_list [else_clause]

(14) rule_while = 'while' compound_list 'do' compound_list 'done' while true do echo a done 

(15) rule_until = 'until' compound_list 'do' compound_list 'done'

(16) rule_for = 'for' WORD ( [';'] | [ {'\n'} 'in' { ( WORD | SUBSHELL ) } ( ';' | '\n' ) ] )
{'\n'} 'do' compound_list 'done'

(17) compound_list = {'\n'} and_or { ( ';' | '\n' ) {'\n'} and_or } [';'] {'\n'}

(18) simple_command =
          prefix {prefix}
(19)    | {prefix} ( WORD | SUBSHELL ) { element }

(20) prefix =
        redirection
        | ASSIGNEMENT_WORD

(21) redirection = [IONUMBER] ( '>' | '<' | '>>' | '>&' | '<&' | '>|' | '<>' )
( WORD | SUBSHELL )

(22) element =
        ( WORD | SUBSHELL )
        | redirection
*/

/* ============= FONCTION DE DEPART ============= */

struct AST *input(struct lexer **lexer);

/* ============= PARSER ============= */

struct AST *list(struct lexer **lexer);
struct AST *and_or(struct lexer **lexer);
struct AST *pipeline(struct lexer **lexer);
struct AST *command(struct lexer **lexer);
struct AST *shell_command(struct lexer **lexer);
struct AST *rule_if(struct lexer **lexer);
struct AST *else_clause(struct lexer **lexer);
struct AST *compound_list(struct lexer **lexer);
struct AST *simple_command(struct lexer **lexer);
struct AST *element(struct lexer **lexer);
struct AST *redirection(struct lexer **lexer);
struct AST *prefix(struct lexer **lexer);
struct AST *rule_for(struct lexer **lexer);
struct AST *rule_while(struct lexer **lexer);
struct AST *rule_until(struct lexer **lexer);
struct AST *funcdec(struct lexer **lexer);

#endif /* ! PARSER_H */
