#include "ast.h"

#include <stdlib.h>
/**
 * Alloue et initialise un nouveau nœud AST avec la règle et le contenu donnés.
 * Le nœud est créé sans enfants (count_children = 0, children = NULL).
 * Retourne un pointeur vers le nœud créé.
 */
struct AST *create_ast(enum rule rule, char *content)
{

    struct AST *start = malloc(sizeof(struct AST));

    start->rule = rule;
    start->content = content;
    start->count_children = 0;
    start->children = NULL;
    start->max_children = 0;
    start->is_neg = 0;

    return start;
}

/**
 * Ajoute un enfant au nœud AST en gérant l'allocation dynamique du tableau.
 * Double la capacité du tableau lorsqu'il est plein.
 * Retourne le pointeur vers le nœud parent (root).
 */

struct AST *add_children(struct AST *root, struct AST *child)
{

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
        root->children =
            realloc(root->children, root->max_children * sizeof(struct AST *));
    }

    root->children[root->count_children] = child;
    root->count_children += 1;

    return root;
}

/**
 * Duplique récursivement un nœud AST et tous ses enfants.
 * Copie également le contenu avec strdup si non-NULL.
 * Retourne un pointeur vers le nouvel arbre dupliqué.
 */
struct AST *dup_ast(struct AST *root)
{
	struct AST *dup = malloc(sizeof(struct AST));
	memcpy(dup, root, sizeof(struct AST));

	dup->children = malloc(sizeof(struct AST*) * root->max_children);
	if(root->content)
		dup->content = strdup(root->content);

	for(int i = 0; i<root->count_children; i++)
	{
		dup->children[i] = dup_ast(root->children[i]);
	}
	return dup;
}

/**
 * Libère récursivement la mémoire d'un nœud AST et de tous ses enfants.
 * Libère le contenu, le tableau d'enfants, puis le nœud lui-même.
 * Ne fait rien si root est NULL.
 */

void destroy_AST(struct AST *root)
{ // detruit l'AST (free)
    if (root == NULL)
        return;
    for (int i = 0; i < root->count_children; i++)
        destroy_AST(root->children[i]);

    free(root->content);
    free(root->children);
    free(root);
}

void destroy_AST_void(void *root)
{
	destroy_AST(root);
}
