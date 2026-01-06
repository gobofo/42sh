#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "../ast/ast.h"

void print_ast_rec(struct AST *ast, FILE *stream)
{
    switch (ast->type)
    {
    case INPUT:
        print_ast_rec(ast->children[0], stream);
        break;

    case LIST:
        for (int i = 0; i < ast->count_children; i++)
        {
            print_ast_rec(ast->children[i], stream);
            fprintf(stream, "\n");
        }
        break;

    case AND_OR:
        if (ast->children[0]->type == PIPELINE)
        {
            print_ast_rec(ast->children[0], stream);
        }
        break;
    case PIPELINE:
        if (ast->children[0]->type == COMMAND)
        {
            print_ast_rec(ast->children[0], stream);
        }
        break;
    case COMMAND:
        if (ast->children[0]->type == SIMPLE_COMMAND)
        {
            fprintf(stream, "command : ");
            print_ast_rec(ast->children[0], stream);
            break;
        }
        if (ast->children[0]->type == SHELL_COMMAND)
        {
            print_ast_rec(ast->children[0], stream);
        }

        break;
    case SHELL_COMMAND:
        if (ast->children[0]->type == RULE_IF)
        {
            print_ast_rec(ast->children[0], stream);
        }

        break;
    case RULE_IF:
        fprintf(stream, "if { ");
        print_ast_rec(ast->children[0], stream); // compound list

        fprintf(stream, "} ; then { ");
        print_ast_rec(ast->children[1], stream); // compound list

        fprintf(stream, "};");

        if (ast->count_children == 3)
        {
            print_ast_rec(ast->children[2], stream);
        }

        break;
    case ELSE_CLAUSE:
        break;
    case COMPOUND_LIST:
        break;
    case SIMPLE_COMMAND:
        break;
    case ELEMENT:
        break;
    default:
        break;
    }
}

void print_ast_main(struct AST *ast)
{
    char *buffer = NULL;
    size_t size = 0;

    FILE *stream = open_memstream(&buffer, &size);

    print_ast_rec(ast, stream);

    fclose(stream);
    free(buffer);
}
