#define _POSIX_C_SOURCE 200809L 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../ast/ast.h"
#include "../execution/execution.h"

struct AST *create_arg(char *str) {
    return create_ast(AST_VALUE, strdup(str)); 
}

int main(void) {
    printf("========================================\n");
    printf("   TEST MANUEL DE EXECUTE_AST\n");
    printf("========================================\n\n");

	printf("--- Test 1 : Commande Simple (echo Hello World) ---\n");
    
    struct AST *cmd_echo = create_ast(AST_CMD, NULL);
    
    add_children(cmd_echo, create_arg("echo"));
    add_children(cmd_echo, create_arg("Hello"));
    add_children(cmd_echo, create_arg("World"));

    execute_ast(cmd_echo); 
    printf("\n");

	printf("--- Test 2 : Condition (if true; then echo Dans le bon; fi) ---\n");

    struct AST *node_if = create_ast(AST_IF, NULL);

    struct AST *cond = create_ast(AST_CMD, NULL);
    add_children(cond, create_arg("true"));

    struct AST *then_block = create_ast(AST_CMD, NULL);
    add_children(then_block, create_arg("echo"));
    add_children(then_block, create_arg("Dans le bon"));

    add_children(node_if, cond);
    add_children(node_if, then_block);

    execute_ast(node_if);
    printf("\n");

    return 0;
}
