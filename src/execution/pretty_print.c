#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "../ast/ast.h"

const char *get_string_of_node(struct AST *ast)
{
    switch (ast->rule)
    {
    case AST_IF:
        return "IF";
    case AST_CMD:
        return "CMD";
    case AST_LIST:
        return "LIST";
    case AST_VALUE:
        return ast->content;
    default:
        return "";
    }
}

void add(FILE *f, const char *texte)
{
    fprintf(f, "%s", texte);
}
void parser_print_rec(struct AST *ast, FILE *f)
{
    const char *node = get_string_of_node(ast);

    for (int i = 0; i < ast->count_children; i++)
    {
        const char *child = get_string_of_node(ast->children[i]);
        add(f, "		");
        add(f, node);
        add(f, " -> ");
        add(f, child);
        add(f, ";\n");
    }

    for (int i = 0; i < ast->count_children; i++)
    {
        parser_print_rec(ast->children[i], f);
    }
}

void parser_print(struct AST *ast)
{
    if (ast == NULL)
        return;
    FILE *f = fopen("AST.txt", "w"); // ecrase le fichier

    add(f, "digraph Arbre {\n");
    parser_print_rec(ast, f);
    add(f, "}");

    fclose(f);
}
