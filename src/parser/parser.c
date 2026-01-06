#include "parser.h"

#include <stdio.h>

#include "../ast/ast.h"
//#include "../lexer/lexer.h"

/*

program: list_statement EOF

list_statement: { statement }

statement: if_satement [ ( SEMICOLON | NEWLINE ) ]
	| true_false_statement [ ( SEMICOLON | NEWLINE ) ]
	| echo_statement [ ( SEMICOLON | NEWLINE ) ]

if_satement: IF condition ( SEMICOLON | NEWLINE ) THEN statement ( SEMICOLON | NEWLINE ) 
\		{ ELIF condition ( SEMICOLON | NEWLINE ) THEN statement ( SEMICOLON | NEWLINE ) } 
\		[ ELSE statement ( SEMICOLON | NEWLINE ) ] FI

condition: 'true'
			| 'false'
			| echo_statement

builtin: echo_statement

echo_statement: 'echo' { value }

value: WORD

*/

struct AST *program(struct node *node);
struct AST *list_statement(struct node *node);
struct AST *statement(struct node *node);
struct AST *if_statement(struct node *node);
struct AST *condition(struct node *node);
struct AST *builtin(struct node *node);
struct AST *echo_statement(struct node *node);
struct AST *value(struct node *node);








void print_parser(void)
{
    print_lexer();
    printf("parser !!!\n");
}
