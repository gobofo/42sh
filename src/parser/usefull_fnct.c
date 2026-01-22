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

struct token *donne_token(struct lexer *lexer){

    return lexer->current;

}

enum types donne_type(struct lexer *lexer){
    return lexer->current->type;
}

char *donne_content(struct lexer *lexer){
    return lexer->current->content;
}

char *donne_entre_paren(struct token *token){
    
    char *res = malloc(sizeof(char) * (strlen(token->content) - 2)); //ya le \0

    memcpy(res, token->content + 2, strlen(token->content) - 3); /// $(aaaaa) strlen = 8
    res[strlen(token->content) - 3] = '\0';

    return res;
}

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


bool verif_subshell(struct lexer *lexer){

    pid_t pid = fork(); //fork

    if (pid < 0){
        return false;
    }

    else if (pid == 0){ //le fils

        char *content = donne_entre_paren(donne_token(lexer)); //contenu entre les paren
        char *command[] = { "./src/42sh", "-c", content }; //nouvelle cmd 

        if (my_42sh_verif(3, command)){ //le parsing est bon
            free(content);
            _exit(0);
        }

        free(content);//le parsing est mauvais
        _exit(1);

    }

    else{

        int wstatus;
        int child_pid = waitpid(pid, &wstatus, 0); //attend l'enfant

        if (child_pid == -1) //err
        {
            return false;
        }

        if (WIFEXITED(wstatus) == 1)//err
            return false;

    }

    return true;

}