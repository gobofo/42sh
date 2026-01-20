#include "parser.h"

#include <stdbool.h>
#include <string.h>

#include "../ast/ast.h"
#include "../lexer/lexer.h"
#include "../token.h"
// #include "../lexer/lexer.h"

/* === GRAMMAR === */

/*

    input =
(1)     list '\n'
(2)    | list EOF
(3)    | '\n'
(4)    | EOF

(5) list = and_or { ';' and_or } [ ';' ]

(6) and_or = pipeline { ( '&&' | '||' ) {'\n'} pipeline }

(7) pipeline = [!] command { '|' {'\n'} command }

    command =
(8)    simple_command
(9)    | shell_command {redirection}
       |funcdec { redirection }


(10) shell_c
    '{' compound_list '}'
   |'(' compound_list ')'
   | rule_if
   | rule_while
   | rule_until
   | rule_for

funcdec = WORD '(' ')' {'\n'} shell_command ;

(11) rule_if = 'if' compound_list 'then' compound_list [else_clause] 'fi'

    else_clause =
(12)    'else' compound_list
(13)    | 'elif' compound_list 'then' compound_list [else_clause]

(14) rule_while = 'while' compound_list 'do' compound_list 'done' while true do echo a done 

(15) rule_until = 'until' compound_list 'do' compound_list 'done'

(16) rule_for = 'for' WORD ( [';'] | [ {'\n'} 'in' { WORD } ( ';' | '\n' ) ] )
{'\n'} 'do' compound_list 'done'

(17) compound_list = {'\n'} and_or { ( ';' | '\n' ) {'\n'} and_or } [';'] {'\n'}

(18) simple_command =
          prefix {prefix}
(19)    | {prefix} WORD { element }

(20) prefix =
        redirection
        | ASSIGNEMENT_WORD

(21) redirection = [IONUMBER] ( '>' | '<' | '>>' | '>&' | '<&' | '>|' | '<>' )
WORD

(22) element =
        WORD
        | redirection
*/

struct AST *input(struct token **token);
static struct AST *list(struct token **token);
static struct AST *and_or(struct token **token);
static struct AST *pipeline(struct token **token);
static struct AST *command(struct token **token);
static struct AST *shell_command(struct token **token);
static struct AST *rule_if(struct token **token);
static struct AST *else_clause(struct token **token);
static struct AST *compound_list(struct token **token);
static struct AST *simple_command(struct token **token);
static struct AST *element(struct token **token);
static struct AST *redirection(struct token **token);
static struct AST *prefix(struct token **token);
static struct AST *rule_for(struct token **token);
static struct AST *rule_while(struct token **token);
static struct AST *rule_until(struct token **token);
static struct AST *funcdec(struct token **token);

int parse_body_if(struct AST **ast, struct token **token);

static bool is_valid_word(struct token *token)
{
    if (token == NULL)
    {
        return false;
    }

    enum types type = token->type;

    return type != NEWLINE && type != AND && type != OR && type != SEMICOLON
        && type != PIPE && type != REDIR;
}

static struct token *eat(struct token *token)
{
    free_token(token);
    return get_token(NULL);
}

static void eat_newlines(struct token **token)
{
    while (*token && (*token)->type == NEWLINE)
        *token = eat(*token);
}

//    input =
//(1)     list '\n'
//(2)    | list EOF
//(3)    | '\n'
//(4)    | EOF

struct AST *input(struct token **token)
{
    if (*token == NULL || (*token)->type == NEWLINE)  //regle 3 et 4
    {
        free_token(*token);
        return create_ast(AST_LIST, NULL);
    }

    if (!first_list(*token)){ //si c pas une liste = erreur
        free_token(*token);
        return NULL;
    }

    struct AST *ast = list(token); //creer la list
    if (ast == NULL) //pb dans la list
    {
        free_token(*token);
        return NULL;
    }

    if (!follow_list(*token)) //pas \n ou EOF
    {
        free_token(*token);
        destroy_AST(ast);
        return NULL;
    }

    free_token(*token); // si \NEWLINE
    return ast;
}

//(3) list = and_or { ';' and_or } [ ';' ]
static struct AST *list(struct token **token)
{
    struct AST *ast = create_ast(AST_LIST, NULL);

    if (!first_and_or(*token)){//pas debut du and_or
        goto err;
    }

    struct AST *child = and_or(token);

    if (child == NULL){//remonte le pb
        goto err;
    }

    ast = add_children(ast, child);

    if (follow_list(*token)){//pas de ;
        return ast;
    }

    if ((*token)->type == SEMICOLON){
        *token = eat(*token);

        if (follow_list(*token)){ // cas [ ; ]
            return ast;
        }
    }

    else { //pas de semicolon
        goto err;
    }

    while (first_and_or(*token)){ //first de and_or

        struct AST *child = and_or(token);
        if (child == NULL) //err remonter
        {
            goto err;
        }
        ast = add_children(ast, child);

        if (follow_list(*token))//fin de la liste
        {
            return ast;
        }

        if ((*token)->type == SEMICOLON)//si ya ;
        {
            *token = eat(*token);
        }

        if (follow_list(*token))//fin de la liste
        {
            return ast;
        }

    }

err:
    destroy_AST(ast);
    return NULL;
}
// 32 ligne
//

//(6) and_or = pipeline { ( '&&' | '||' ) {'\n'} pipeline }

static struct AST *and_or(struct token **token)
{
    
    struct AST *ast = NULL;

    if (!first_pipeline(*token)){//pas un pipeline
        goto err;
    }

    ast = pipeline(token);

    if (ast == NULL){//remonte l'erreur
        goto err;
    }

    while (*token && ((*token)->type == AND || (*token)->type == OR)){

        struct AST *ast_op;

        if ((*token)->type == AND)
        {
            ast_op = create_ast(AST_AND, NULL); //le AND 
        }
        else
        {
            ast_op = create_ast(AST_OR, NULL); //le OR
        }

        *token = eat(*token);

        eat_newlines(token); //pour {\n}

        if (!first_pipeline(*token)){
            destroy_AST(ast_op);
            goto err;
        }

        ast_op = add_children(ast_op, ast);
        ast = pipeline(token);

        if (ast == NULL) //remonte l'erreur
        {
            destroy_AST(ast_op);
            goto err;
        }

        ast_op = add_children(ast_op, ast);
        ast = ast_op; // on change comme ca les AST de and et OR se construisent a l'envers

    }

    if (follow_and_or(*token)){
        return ast;
    }

err:
    destroy_AST(ast);
    return NULL;

}

//(7) pipeline = [!] command { '|' {'\n'} command }
static struct AST *pipeline(struct token **token)
{
    struct AST *ast = create_ast(AST_PIPELINE, NULL);
    if ((*token)->type == NEG)
    {
        ast->is_neg = 1;
        *token = eat(*token);
    }

    if (!first_command(*token)){ //pas une commande
        goto err;
    }

    struct AST *children = command(token);

    if (children == NULL) //remonte l'erreur 
    {
        goto err;
    }

    ast = add_children(ast, children);

    if (follow_pipeline(*token)){
        return ast;
    }

    while ((*token)->type == PIPE)
    {
        *token = eat(*token);
        if (*token == NULL)
            goto err;

        eat_newlines(token);

        if (!first_command(*token)){
            goto err;
        }

        struct AST *children = command(token);

        if (children == NULL) //remonte l'erreur
        {
            goto err;
        }

        ast = add_children(ast, children);

        if (follow_pipeline(*token)){
            return ast;
        }
    }

err:
    destroy_AST(ast);
    return NULL;
}

// 32
//

//         command =
//(8)    simple_command
//(9)    | shell_command {redirection}
//       | funcdec {redirection} 

// entation de la fonction libre indique explicitement qu'il est possible de
// lui passer un pointeur n

static struct AST *command(struct token **token)
{
    struct AST *ast=NULL;

    int _redir=0;
    if (first_shell_command(*token))//cas de shell command
    {
        _redir=1;//pour la suite pour evite de mettre un while pour chaque 
        ast = create_ast(AST_SHELL_CMD, NULL);

        struct AST *ast_shell = shell_command(token);

        if (ast_shell == NULL)//remonte l'erreur
          goto err;

        ast = add_children(ast, ast_shell);
    }

    else if((*token)->type==WORDS){//pour le funcdec

      struct token *look_ahead=next_token(NULL);//on regarde un devant

      if(look_ahead && look_ahead->type==L_PAREN){ //si c une parenthese c un funcdec

        _redir=1;//pareil que pour le shl_cmd

        ast = create_ast(AST_FUNC, strdup((*token)->content));
        struct AST *ast_func = funcdec(token);

        if (ast_func == NULL)
          goto err;

        ast = add_children(ast, ast_func);
      }

    }

    if(_redir==0){ //c une simple commande

      if(!first_simple_command(*token)) //verifie que c une simple commande
        goto err;

      return simple_command(token);
    }

    while (first_redirection(*token)) //mager toutes les redirections
    {

      struct AST *ast_redir = redirection(token);

      if (ast_redir == NULL)//remonte l'err
        goto err;

      ast = add_children(ast, ast_redir);
    }

    if (follow_command(*token))
      return ast;

err:
    destroy_AST(ast);
    return NULL;

} // 22

//(10) shell_c
//    '{' compound_list '}'
//   |'(' compound_list ')'
//   | rule_if
//   | rule_while
//   | rule_until
//   | rule_for

static struct AST *shell_command(struct token **token)
{
    if (first_rule_if(*token)) //cas if
    {
        struct AST *ast = rule_if(token);
        if (ast == NULL)
            return NULL;
        return ast;
    }
    else if (first_rule_while(*token))//cas while 
    {
        struct AST *ast = rule_while(token);
        if (ast == NULL)
            return NULL;
        return ast;
    }
    else if (first_rule_until(*token))//cas until
    {
        struct AST *ast = rule_until(token);
        if (ast == NULL)
            return NULL;
        return ast;
    }
    else if (first_rule_for(*token))//cas for
    {
        struct AST *ast = rule_for(token);
        if (ast == NULL)
            return NULL;
        return ast;
    }
    else if (*token != NULL && ((*token)->type == L_BRACE || (*token)->type == L_PAREN)){ //cas 1 et 2

        bool is_paren = (*token)->type == L_PAREN;

        *token = eat(*token);

        if (!first_compound_list(*token)){ //pas une coumpound list
            return NULL;
        }

        struct AST *ast = compound_list(token);

        if (ast == NULL){//remonte l'erreur
            return NULL;
        }

        if (*token == NULL || ((*token)->type != R_BRACE && (*token)->type != R_PAREN)){ //sans parenthese / bracket a la fin
            goto err;
        }

        if (((*token)->type == R_PAREN && !is_paren) || ((*token)->type == R_BRACE && is_paren)){ //pas le meme correspondance ( } ou { )
            goto err;
        }

        *token = eat(*token);

        return ast;

err:
        destroy_AST(ast);
        return NULL;

    }
    else
    {
        return NULL;
    }
} // 22


//funcdec = WORD '(' ')' {'\n'} shell_command ;

static struct AST *funcdec(struct token **token){

    if (!is_valid_word(*token)){ //pas un bon mot
        return NULL;
    }

    *token = eat(*token);

    if (*token == NULL || (*token)->type != L_PAREN){ //pas paren
        return NULL;
    }

    *token = eat(*token);

    if (*token == NULL || (*token)->type != R_PAREN){ //pas paren
        return NULL;
    }

    *token = eat(*token);

    eat_newlines(token);

    return shell_command(token);

}

//(9) rule_if = 'if' compound_list 'then' compound_list [else_clause] 'fi'

static struct AST *rule_if(struct token **token)
{

    struct AST *ast = create_ast(AST_IF, NULL);

    if ((*token)->type != IF){ //pas un if
        goto err;
    }
    
    *token = eat(*token);

    if (!first_compound_list(*token)){ //pas une compound list
        goto err;
    }

    struct AST *child = compound_list(token);
    if (child == NULL) //remonte l'err
    {
        goto err;
    }
    ast = add_children(ast, child);

    if (*token == NULL || (*token)->type != THEN)
    {
        goto err;
    }

    *token = eat(*token);

    if (!first_compound_list(*token)){ //pas une compound list
        goto err;
    }

    struct AST *child_second = compound_list(token); //envoi dans compound list
    if (child_second == NULL)
    {
        goto err;
    }
    ast = add_children(ast, child_second);

    if (first_else_clause(*token))
    { // esle clause existe
        struct AST *child_third = else_clause(token);
        if (child_third == NULL)
        {
            goto err;
        }
        ast = add_children(ast, child_third);
    }

    if (*token == NULL || (*token)->type != FI)
    { // verifie la GRAMMAR FIN
        goto err;
    }

    *token = eat(*token); //on mange le fi

    return ast;

err:
    destroy_AST(ast);
    return NULL;
} // 37

// else_clause =
//(10)    'else' compound_list
//(11)    | 'elif' compound_list 'then' compound_list [else_clause]

static struct AST *else_clause(struct token **token)
{
    struct AST *ast = NULL;

    if ((*token)->type == ELSE)//cas du else
    {
        *token = eat(*token);

        if (!first_compound_list(*token))
        {
            return NULL;
        }
        ast = compound_list(token);
        return ast;
    }
    else if ((*token)->type == ELIF)//cas elif mais au final c comme un if
    {
        ast = create_ast(AST_IF, NULL);
        *token = eat(*token);

        if (!first_compound_list(*token))//pas une compound list
        {
            goto err;
        }

        struct AST *child = compound_list(token);
        if (child == NULL)//remnte l'err
        {
            goto err;
        }
        ast = add_children(ast, child);

        if (*token == NULL || (*token)->type != THEN)//c pas then
        {
            goto err;
        }

        *token = eat(*token);//eat le then

        if (!first_compound_list(*token)){//pas un compound list
            goto err;
        }

        struct AST *child_second = compound_list(token);
        if (child_second == NULL)//remonte l'erreur
        {
            goto err;
        }
        ast = add_children(ast, child_second);

        if (first_else_clause(*token))
        { // esle clause existe
            struct AST *child_third = else_clause(token);
            if (child_third == NULL)//remonte l'err
            {
                goto err;
            }
            ast = add_children(ast, child_third);
        }
        return ast;
    }

err:
    destroy_AST(ast);
    return NULL;
} // 37

//(14) rule_while = 'while' compound_list 'do' compound_list 'done'
static struct AST *rule_while(struct token **token)
{
    struct AST *ast = create_ast(AST_WHILE, NULL);

    if ((*token)->type != WHILE){//pas de while au debut
        goto err;
    }

    *token = eat(*token);///mange le while

    if (!first_compound_list(*token)){//pas un first de compound list
        goto err;
    }

    struct AST *child = compound_list(token);

    if (child == NULL){//remonte l'erreur
        goto err;
    }

    ast = add_children(ast, child);

    if (*token == NULL || (*token)->type != DO) // pas de DO
        goto err;

    *token = eat(*token);

    if (!first_compound_list(*token)){//pas un first de compound list
        goto err;
    }

    struct AST *child_second = compound_list(token);

    if (child_second == NULL){//remonte l'erreur
        goto err;
    }

    ast = add_children(ast, child_second);

    if (*token == NULL || (*token)->type != DONE) // pas de done
        goto err;

    *token = eat(*token);

    return ast;

err:
    destroy_AST(ast);
    return NULL;

}

//(15) rule_until = 'until' compound_list 'do' compound_list 'done'
static struct AST *rule_until(struct token **token)
{
    struct AST *ast = create_ast(AST_UNTIL, NULL);

    if ((*token)->type != UNTIL){//pas de unitl au debut
        goto err;
    }

    *token = eat(*token);///mange le until

    if (!first_compound_list(*token)){//pas un first de compound list
        goto err;
    }

    struct AST *child = compound_list(token);

    if (child == NULL){//remonte l'erreur
        goto err;
    }

    ast = add_children(ast, child);

    if (*token == NULL || (*token)->type != DO) // pas de DO
        goto err;

    *token = eat(*token);

    if (!first_compound_list(*token)){//pas un first de compound list
        goto err;
    }

    struct AST *child_second = compound_list(token);

    if (child_second == NULL){//remonte l'erreur
        goto err;
    }

    ast = add_children(ast, child_second);

    if (*token == NULL || (*token)->type != DONE) // pas de done
        goto err;

    *token = eat(*token);

    return ast;

err:
    destroy_AST(ast);
    return NULL;
}

//(16) rule_for = 'for' WORD ( [';']
//                          | [ {'\n'} 'in' { WORD } ( ';' | '\n' ) ] )
//{'\n'} 'do' compound_list 'done'

static struct AST *rule_for(struct token **token)
{
    struct AST *ast = create_ast(AST_FOR, NULL);

    if ((*token)->type != FOR) //pas de for au debut
        goto err;

    *token = eat(*token); //mange le for

    if (!is_valid_word(*token)) //pas bon word mais peut etre if, do (voir fonction is_valid_word)
        goto err;

    struct AST *var_name = create_ast(AST_VALUE, strdup((*token)->content)); //nom de la variable
    ast = add_children(ast, var_name);
    *token = eat(*token);

    if (*token == NULL)//si ya rien c erreur
        goto err;

    // Cas 1
    if ((*token)->type == SEMICOLON)
    {
        *token = eat(*token);
    }
    else // Cas 2
    {
        eat_newlines(token); //enleve le {\n}

        if (*token != NULL && (*token)->type == IN) //cas avec le in
        {
            *token = eat(*token);

            if (*token == NULL)//eviter pb
                goto err;

            while (*token != NULL && (*token)->type != SEMICOLON
                   && (*token)->type != NEWLINE) //va mager tous les words
            {

                if (!is_valid_word(*token)) //c pas un mots alors c une err
                    goto err;

                struct AST *word =
                    create_ast(AST_VALUE, strdup((*token)->content)); //prend la valeur du mot
                ast = add_children(ast, word);

                *token = eat(*token);
            }

            if (*token == NULL
                || ((*token)->type != SEMICOLON && (*token)->type != NEWLINE)) //problem de grammaire
                goto err;

            *token = eat(*token);//on eat le ; ou le \n
        }
    }

    eat_newlines(token);//enleve le {\n}
    if (*token == NULL || (*token)->type != DO) //pas de do
        goto err;

    *token = eat(*token);//mange le do

    if (!first_compound_list(*token))//pas first de compound lst
        goto err;

    struct AST *body = compound_list(token);
    if (body == NULL)//remonte l'erreur
        goto err;

    ast = add_children(ast, body);

    if (*token == NULL || (*token)->type != DONE) //pas de done
        goto err;

    *token = eat(*token); //mange le done

    return ast;

err:
    destroy_AST(ast);
    return NULL;
}

//(17) compound_list = {'\n'} and_or { ( ';' | '\n' ) {'\n'} and_or } [';'] {'\n'}

static struct AST *compound_list(struct token **token)
{
    struct AST *ast = create_ast(AST_LIST, NULL);

    eat_newlines(token);//pour les {\n}

    if (!first_and_or(*token)) //pas un and or
        goto err;

    struct AST *child = and_or(token);
    if (child == NULL)//remonte l'err
        goto err;

    ast = add_children(ast, child);
    
    while (*token != NULL
           && ((*token)->type == SEMICOLON || (*token)->type == NEWLINE))
    {
        *token = eat(*token); //mange le /n ou le ;
        eat_newlines(token); //pour le {\n}

        if (*token == NULL || follow_compound_list(*token))//c la fin
            break;

        if (!first_and_or(*token)) //c pas un and or
            goto err;

        child = and_or(token);
        if (child == NULL)//remonte l'err
            goto err;
        ast = add_children(ast, child);
    }

    //==================== PAS UTILE =========================

    if (*token != NULL && (*token)->type == SEMICOLON)
        *token = eat(*token);

    eat_newlines(token);

    //==================== PAS UTILE =========================

    return ast;

err:
    destroy_AST(ast);
    return NULL;
}

//(18) simple_command =
//         prefix {prefix}
//(19)    | {prefix} WORD { element }

static struct AST *simple_command(struct token **token)
{
    struct AST *ast = create_ast(AST_SIMPLE_CMD, NULL);

    int pref = 0;

    while (*token && first_prefix(*token)) // va manger tous les prefix du debut
    {
        pref = 1;

        struct AST *child = prefix(token);
        if (child == NULL)//remonte l'erreur
            goto err;
        ast = add_children(ast, child);
    }

    if (pref == 0 && *token == NULL) //pas de prefix et rien apres
    {
        goto err;
    }

    if (pref == 0 || (*token && is_valid_word(*token))) // regle 2
    {
        struct AST *child;

        if (!is_valid_word(*token)){//pas un mot alors c une erreur
            goto err;
        }
            
        child = create_ast(AST_VALUE, strdup((*token)->content)); //prend la valeur du WORDS

        ast = add_children(ast, child);

        *token = eat(*token);

        while (*token && (first_element(*token) || is_valid_word(*token))) //first de element ou un valid word car element peut etre just word
        {
            struct AST *child_el = element(token);
            if (child_el == NULL)//remonte l'err
                goto err;

            ast = add_children(ast, child_el);
        }

        if (!follow_simple_command(*token))//si ce qui suit n'est pas bon direct erreur
        {
            goto err;
        }
        
        return ast;
    }
    else //cas 1
    {
        if (!follow_simple_command(*token))//si ce qui suit n'est pas bon direct erreur
        {
            goto err;
        }

        return ast;
    }

err:
    destroy_AST(ast);
    return NULL;
} //>34

//(20) prefix =
//        redirection
//        | ASSIGNEMENT_WORD

static struct AST *prefix(struct token **token)
{
    if ((*token)->type == A_WORDS)//cas 2
    {
        struct AST *ast =
            create_ast(AST_ASSIGNEMENT, strdup((*token)->content));//recup la valeur du a_word
        *token = eat(*token);
        return ast;
    }

    else if (first_redirection(*token))//cas 1
    {
        struct AST *ast = redirection(token);
        if (ast == NULL)//remonte l'erreur
        {
            return NULL;
        }
        return ast;
    }

    return NULL;
}

//(21) redirection = [IONUMBER] ( '>' | '<' | '>>' | '>&' | '<&' | '>|' | '<>' )
// WORD

static struct AST *redirection(struct token **token)
{
    struct AST *ast = NULL;

    if (*token == NULL || (*token)->type != REDIR){ //pas une redirection
        goto err;
    }

    ast = create_ast(AST_REDIR, NULL);
    struct AST *ast_val = create_ast(AST_VALUE, strdup((*token)->content));//prend la valeur du redir
    ast = add_children(ast, ast_val);

    *token = eat(*token);

    if (!is_valid_word(*token)){//pas un valid word
        goto err;
    }

    struct AST *ast_val2 = create_ast(AST_VALUE, strdup((*token)->content));//recup la valeur du word
    ast = add_children(ast, ast_val2);

    *token = eat(*token); //mange le word
    
    return ast;

err:
    destroy_AST(ast);
    return NULL;
}

//(22) element =
//        WORD
//        | redirection

static struct AST *element(struct token **token)
{
    if (is_valid_word(*token))//cas 1
    {
        struct AST *ast = create_ast(AST_VALUE, strdup((*token)->content));//recup la valeur du word
        *token = eat(*token);//mange le word
        return ast;
    }

    if ((*token)->type == REDIR)//cas 2
    {
        struct AST *ast = redirection(token);
        if (ast == NULL)//remonte l'erreur
        {
            return NULL;
        }
        return ast;
    }

    return NULL;
}
