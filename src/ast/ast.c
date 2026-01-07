#include "ast.h"

#include <stdlib.h>


struct AST *create_ast(enum rule rule, char *content)
{ // renvoi un arbre avec uniquement la racine "type"

    struct AST *start = malloc(sizeof(struct AST));

    start->rule = rule;
    start->content = content;
    start->count_children = 0;
    start->children = NULL;
    start->max_children = 0;

    return start;
}

struct AST *add_children(struct AST *root, struct AST *child)
{ // ajoute un enfant en s'occupant des realloc

    if (root->count_children >= root->max_children)
    { // pour les realloc

        if (root->max_children == 0)
        {
            root->max_children = 2;
        }

        else
        {
            root->max_children *= 2;
        }

        root->children = realloc(root->children, root->max_children);
    }

    root->children[root->count_children] = child;
    root->count_children += 1;

    return root;
}

void destroy_AST(struct AST *root)
{ // detruit l'AST (free)

    for (int i = 0; i < root->count_children; i++)
    {
        destroy_AST(root->children[i]);
    }

    free(root->children);
    free(root);
}
