#include "usefull_fnct.h"

//renvoi true si le mot est un valid word

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

//donne le current token

struct token *get_current_token(struct lexer *lexer){

    return lexer->current;

}

//donne le type du current token

enum types get_current_type(struct lexer *lexer){
    return lexer->current->type;
}

//donne le content du current token

char *get_current_content(struct lexer *lexer){
    return lexer->current->content;
}

//donne ce qu'il y a entre les parenthese $(Bravo Nils) => Bravo Nils

char *extract_parentheses_content(struct token *token, int *cmpt){
    
    char *res = malloc( 8 * sizeof(char));
    int capacity = 8;

    char *content = token->content;

    while (content[*cmpt] != '\0' && content[*cmpt] != '('){ //je vais jusqu'a la parenthese
        *cmpt += 1;
    }

    if (content[*cmpt] == '\0'){

        free(res);
        return NULL;

    }
        
    
    *cmpt += 1;
    int nb_parenthese = 1;
    int i = 0;

    while (content[*cmpt] != '\0' && nb_parenthese != 0){

        if (content[*cmpt] == ')'){

            nb_parenthese -= 1;

            if (nb_parenthese != 0){
                
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

        else{

            if (i >= capacity){//pour le realloc

                capacity *= 2;
                res = realloc(res, capacity * sizeof(char));

            }

            res[i] = content[*cmpt];
            i += 1;

        }

        *cmpt += 1;

    }

    if (i >= capacity){//pour le realloc

        capacity *= 2;
        res = realloc(res, capacity * sizeof(char));

    }

    res[i] = '\0';

    return res;

}

//permet de verifier si une chaine de caractere est valide dans le parser

bool my_42sh_verif(int argc, char *argv[])
{
    FILE *file = get_input_file(argc, argv);

    if (file == NULL)
    {
        return false;
    }
    struct lexer *lexer = init_lexer(file);
    if (!lexer)
    {
        return false;
    }

    while (lexer->current != NULL)
    {
        struct AST *ast = input(&lexer);

        if (ast == NULL)
        {
            free_lexer(lexer);

            fclose(file);

            return false;
        }

        destroy_AST(ast);

        free_token(lexer->current);
        lexer = get_token(lexer);
    }

    
    free_lexer(lexer);
    fclose(file);

    return true;
}

//permet de verifier si les $(echo a) sont valide

bool verif_subshell(struct lexer *lexer){

    pid_t pid = fork(); //fork

    if (pid < 0){
        return false;
    }

    else if (pid == 0){ //le fils

        int cmpt = 0;

        int len = strlen(lexer->current->content);

        while (cmpt < len){

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

        _exit(0);

    }

    else{

        int wstatus;
        waitpid(pid, &wstatus, 0); //attend l'enfant
        if (WEXITSTATUS(wstatus) == 1)//err
            return false;

        return true;

    }


}
