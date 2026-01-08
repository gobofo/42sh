#ifndef AST_H
#define AST_H

enum rule
{

    AST_LIST,
    AST_CMD,
    AST_IF,
    AST_VALUE

};

struct AST
{
    enum rule rule; // rules de la node
    char *content; // valeur de la node (si besoin)
    struct AST **children; // tous les enfants
    int count_children; // nb d'enfant
    int max_children; // pour pouvoir realloc
};

struct AST *create_ast(enum rule rule, char *content);
struct AST *add_children(struct AST *root, struct AST *child);
void destroy_AST(struct AST *root);

#endif /* ! AST_H */
