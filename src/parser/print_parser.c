#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "../ast/ast.h"
#include "parser.h"

char *get_string_of_node(struct AST *ast)
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

char *next(int suivant)
{
    static int c = 1;
    if (suivant)
    {
        c++;
    }
    char *buffer = malloc(10);
    sprintf(buffer, "%d", c);
    return buffer;
}

void add(FILE *f, const char *texte)
{
    fprintf(f, "%s", texte);
}
void parser_print_rec(struct AST *ast, FILE *file_node, FILE *file_def_node)
{
    char *node = next(0);

    fprintf(file_def_node, "%s [label=\"%s\"];\n", node,
            get_string_of_node(ast));

    for (int i = 0; i < ast->count_children; i++)
    {
        char *child = next(1);
        add(file_node, "     ");
        add(file_node, node);
        add(file_node, " -> ");
        add(file_node, child);
        add(file_node, ";\n");
        parser_print_rec(ast->children[i], file_node, file_def_node);
        free(child);
    }
    free(node);
}
void merge_into(FILE *src, FILE *dst)
{
    char buffer[1024];

    while (fgets(buffer, sizeof(buffer), src))
        fputs(buffer, dst);
}

void parser_print(struct AST *ast)
{
    if (ast == NULL)
        return;
    FILE *file_node = fopen("AST_node.txt", "w"); // ecrase le fichier
    FILE *file_def_node = fopen("AST_def_node.txt", "w"); // ecrase le fichier

    add(file_def_node, "digraph Arbre {\n");
    add(file_def_node,
        "node [shape=box, style=filled, fillcolor=lightblue];\n");
    parser_print_rec(ast, file_node, file_def_node);

    fclose(file_node);
    file_node = fopen("AST_node.txt", "r");
    merge_into(file_node, file_def_node);

    add(file_def_node, "}");
    fclose(file_node);
    fclose(file_def_node);
}
