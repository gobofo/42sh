#include "ast/ast.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "IO/input/input.h"
#include "token.h"

#include <stdio.h>

int main(int argc, char *argv[])
{
    FILE * file = getInputFile(argc, argv);

    if (file == NULL){

        fprintf(stderr, "Error: IO\n");
        return 2;

    }

    struct token *tok = get_token(file);

    struct AST *ast;

    if (tok == NULL){

        ast = create_ast(AST_LIST, NULL);

    }

    else {

        ast = input(&tok);

        if (ast == NULL){

            fprintf(stderr, "Error: Parsing\n");
            return 2;

        }

    }

    //execution_ast(ast);

    destroy_AST(ast);

    return 0;
}
