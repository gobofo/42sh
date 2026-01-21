#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "../ast/ast.h"
#include "print_parser.h"

char *get_string_of_node(struct AST *ast)
{
    switch (ast->rule)
    {
    case AST_SHELL_CMD:
        return "SHELL_CMD";
    case AST_WHILE:
        return "WHILE";
    case AST_UNTIL:
        return "UNTIL";
    case AST_FOR:
        return "FOR";
    case AST_AND:
        return "AND";
    case AST_OR:
        return "OR";

    case AST_PIPELINE:
        return "PIPELINE";

    case AST_REDIR:
        return "REDIR";

    case AST_IF:
        return "IF";
    case AST_SIMPLE_CMD:
        return "SIMPLE_CMD";
    case AST_LIST:
        return "LIST";
    case AST_VALUE:
        return ast->content;

    case AST_ASSIGNEMENT:
        return ast->content;
    case AST_FUNC:
        return ast->content;
    default:
        return "";
    }
}
char *get_color_of_node(struct AST *ast)
{
    switch (ast->rule)
    {
    case AST_IF:
        return "#E67E22";
    case AST_SIMPLE_CMD:
        return "#2ECC71";
    case AST_LIST:
        return "#3498DB";
    case AST_VALUE:
        return "#ECF0F1";
    case AST_PIPELINE:
        return "#9B59B6";

    case AST_AND:
        return "#F1C40F";
    case AST_OR:
        return "#F1C40F";

    case AST_REDIR:
        return "#E67E22";

    case AST_UNTIL:
        return "#1ABC9C";
    case AST_WHILE:
        return "#1ABC9C";
    case AST_FOR:
        return "#1ABC9C";
    case AST_SHELL_CMD:
        return "#E74C3C";
    case AST_FUNC:
        return "#E74C3C";

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
    char *buffer = malloc(20);
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

    fprintf(file_def_node, "%s [label=\"%s\" , fillcolor=\"%s\"];\n", node,
            get_string_of_node(ast), get_color_of_node(ast));

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
    FILE *file_def_node = fopen("AST_def_node.dot", "w"); // ecrase le fichier

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
    remove("AST_node.txt");
}
