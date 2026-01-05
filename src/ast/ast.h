#ifndef AST_H
#define AST_H

enum types{

    PROGRAM,
    LIST_STATEMENT,
    STATEMENT,
    IF_STATEMENT,
    CONDITION,
    BUILTIN,
    ECHO_STATEMENT
    VALUES

};

struct AST{

    enum types type; //type de la node
    char *content; //valeur de la node (si besoin)
    struct AST **children; //tous les enfants
    int count_children; //nb d'enfant
    int max_children; //pour pouvoir realloc

};

struct AST *create_ast(enum types type, char *content);
struct AST *add_children(struct AST *root, enum types type, char *content);
void destroy_AST(struct AST *root);
void print_ast(void);

#endif /* ! AST_H */
