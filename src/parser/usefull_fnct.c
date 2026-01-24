#include "usefull_fnct.h"

//renvoi true si le mot est un valid word
//Par exemple dans certaine situation if peut etre considere comme un word (echo if)

bool is_valid_word(struct lexer *lexer)
{
    if (lexer->current == NULL)
    {
        return false;
    }

    enum types type = lexer->current->type;

    return type != NEWLINE && type != AND && type != OR && type != SEMICOLON
        && type != PIPE && type != REDIR && type != L_PAREN && type != R_PAREN;
}

//free le token actuelle et renvoi le suivant
//cette fonction nous premet d'avancer dans les token lors du parsing

struct lexer *eat(struct lexer *lexer)
{
    free_token(lexer->current);
    return get_token(lexer);
}

//mange les {/n}

void eat_newlines(struct lexer **lexer)
{
    while ((*lexer)->current && (*lexer)->current->type == NEWLINE)
        *lexer = eat(*lexer);
}

//return le current token

struct token *get_current_token(struct lexer *lexer){

    return lexer->current;

}

//return le type du current token

enum types get_current_type(struct lexer *lexer){
    return lexer->current->type;
}

//return le content du current token

char *get_current_content(struct lexer *lexer){
    return lexer->current->content;
}

//return ce qu'il y a entre les parenthese $(Bravo Nils) => Bravo Nils
//Prend en parametre le token qui contient le string et aussi cmpt le point de depart a partir
//du quel la fonction doit commencer a chercher

char *extract_parentheses_content(struct token *token, int *cmpt){
    
    char *res = malloc( 8 * sizeof(char)); //stock le resultat
    int capacity = 8; // on utilise un capacity pour pouvoir realloc

    char *content = token->content;

    while (content[*cmpt] != '\0' && content[*cmpt] != '('){ //je vais jusqu'a la parenthese
        *cmpt += 1;
    }

    if (content[*cmpt] == '\0'){ //si il est vide c'est qu'il n'y a pas de parentheses

        free(res);
        return NULL;

    }
        
    
    *cmpt += 1; //incremente pour la (
    int nb_parenthese = 1; //compte les parentheses pour bien prendre la fermante qui correspond
    int i = 0;//compteur pour le res

    while (content[*cmpt] != '\0' && nb_parenthese != 0){ //check tant que c'est pas NULL (evite les buffer overflow)
                                                          //et le nb_paren pour bien s'aretter sur la fermante

        if (content[*cmpt] == ')'){

            nb_parenthese -= 1;

            if (nb_parenthese != 0){ //pas la derniere on la rentre dans le resultat
                
                if (i >= capacity){//pour le realloc

                capacity *= 2;
                res = realloc(res, capacity * sizeof(char));

                }

                res[i] = content[*cmpt];
                i += 1;

            }

        }

        else if (content[*cmpt] == '('){

            nb_parenthese += 1;

            if (i >= capacity){//pour le realloc

                capacity *= 2;
                res = realloc(res, capacity * sizeof(char));

            }

            res[i] = content[*cmpt];
            i += 1;
        }

        else{//caractere simple

            if (i >= capacity){//pour le realloc

                capacity *= 2;
                res = realloc(res, capacity * sizeof(char));

            }

            res[i] = content[*cmpt];
            i += 1;

        }

        *cmpt += 1; //incremente le compteur

    }

    if (i >= capacity){//pour le realloc

        capacity *= 2;
        res = realloc(res, capacity * sizeof(char));

    }

    res[i] = '\0';//ferme le string

    return res;

}

//permet de verifier si une chaine de caractere est valide dans le parser
//ressemble fortement au main mais sans l'execution (la dans un but de verification pour les subshell)

bool my_42sh_verif(int argc, char *argv[])
{
    FILE *file = get_input_file(argc, argv); //recupere le string et le met sous forme de FILE

    if (file == NULL) //erreur
    {
        return false;
    }
    struct lexer *lexer = init_lexer(file); //creer le lexer
    if (!lexer) //le lexing est mauvcais
    {
        return false;
    }

    while (lexer->current != NULL) //boucle pour gerer tous les arbres
    {
        struct AST *ast = input(&lexer); //creer l'ast

        if (ast == NULL) //ast pas bon
        {
            free_lexer(lexer);

            fclose(file);

            return false;
        }

        destroy_AST(ast);

        free_token(lexer->current);
        lexer = get_token(lexer); //passe au token suivant
    }

    
    free_lexer(lexer);
    fclose(file);

    return true; //le parsing est bon
}

//permet de verifier si les $(echo a) sont valide
//va recuperer ce qu'il y a entre les parenthese et, fork() et verfier si
//ce sous string est valid grace a my_42sh_verif

bool verif_subshell(struct lexer *lexer){

    pid_t pid = fork(); //fork

    if (pid < 0){
        return false;
    }

    else if (pid == 0){ //le fils

        int cmpt = 0;

        int len = strlen(lexer->current->content);

        while (cmpt < len){//tant que l'on est pas arriver au bout de tous le content
                           //pour gerer les $(suu)$(aaaa)

            char *content = extract_parentheses_content(get_current_token(lexer), &cmpt); //contenu entre les paren

            if (content == NULL){ //pas de $()
                _exit(0);
            }

            char *command[] = { "./src/42sh", "-c", content, NULL }; //nouvelle cmd 

            if (!my_42sh_verif(3, command)){ //le parsing pas bon
                free(content);
                _exit(1);
            }

            free(content);

        }

        _exit(0); //le res est bon

    }

    else{

        int wstatus;
        waitpid(pid, &wstatus, 0); //attend l'enfant
        if (WEXITSTATUS(wstatus) == 1)//err
            return false;

        return true;

    }


}
