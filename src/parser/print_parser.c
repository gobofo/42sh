#include "../ast/ast.h"
#include "print_parser.h"

struct ast_node nodes[] =  {
	{AST_LIST, "LIST", "#3498DB"},
	{AST_SIMPLE_CMD, "SIMPLE_CMD", "#2ECC71"},
	{AST_SHELL_CMD, "SHELL_CMD", "#E74C3C"},
	{AST_IF, "IF", "#E67E22"},
	{AST_WHILE, "WHILE", "#1ABC9C"},
	{AST_UNTIL, "UNTIL", "#1ABC9C"},
	{AST_FOR, "FOR", "#1ABC9C"},
	{AST_VALUE, "VALUE", "#ECF0F1"},
	{AST_AND, "AND", "#F1C40F"},
	{AST_OR, "OR", "#F1C40F"},
	{AST_PIPELINE, "PIPELINE", "#9B59B6"},
	{AST_REDIR, "REDIR", "#E67E22"},
	{AST_ASSIGNEMENT, "ASSIGNEMENT", "#ECF0F1"},
	{AST_FUNC, "FUNC", "#E74C3C"},
	{AST_SUB, "SUBSHELL", "#E67E22"},
	{0, "", ""}
};

char *get_string_of_node(struct AST *ast)
{

	if (ast->rule != AST_VALUE && ast->rule != AST_ASSIGNEMENT &&
			ast->rule != AST_FUNC)
	{
		return nodes[ast->rule].text;
	}

	return ast->content;
}

char *get_color_of_node(struct AST *ast)
{
	return nodes[ast->rule].color;
}

char *next(int suivant)
{
    static int c = 1;

    if (suivant)
        c++;

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
