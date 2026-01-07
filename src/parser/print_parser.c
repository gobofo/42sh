#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "../ast/ast.h"

const char *get_string_of_node(struct AST *ast)
{
    static int _if = 1;
    static int _cmd = 1;
    static int _list = 1;
    static int _value = 1;

    static char buffer[64];

    switch (ast->rule)
    {
    case AST_IF:
        snprintf(buffer, sizeof(buffer), "IF%d", _if++);
        return buffer;

    case AST_CMD:
        snprintf(buffer, sizeof(buffer), "CMD%d", _cmd++);
        return buffer;

    case AST_LIST:
        snprintf(buffer, sizeof(buffer), "LIST%d", _list++);
        return buffer;

    case AST_VALUE:
        snprintf(buffer, sizeof(buffer), "%s %d", ast->content, _list++);
        return buffer;

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
        add(f, "     ");
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
    add(f, "node [shape=box, style=filled, fillcolor=lightblue];\n");
    parser_print_rec(ast, f);
    add(f, "}");

    fclose(f);
}
