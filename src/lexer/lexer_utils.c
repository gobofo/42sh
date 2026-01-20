#include "lexer_utils.h"

// #############
// #   TOKEN   #
// #############

/**
 * @brief 		Creates a token.
 *
 * The function creates a token from a specific string, that will be the
 * content, and associates the corresponding type.
 *
 * @param str 	The content of the token.
 * 			  	Must not be NULL
 *
 * @return 		Token
 */

static int clause_if(struct token **token, char *str)
{
    if (strcmp(str, "if") == 0)
        (*token)->type = IF;
    else if (strcmp(str, "then") == 0)
        (*token)->type = THEN;
    else if (strcmp(str, "elif") == 0)
        (*token)->type = ELIF;
    else if (strcmp(str, "else") == 0)
        (*token)->type = ELSE;
    else if (strcmp(str, "fi") == 0)
        (*token)->type = FI;
    else
        return 0;

    return 1;
}

static int clause_loop(struct token **token, char *str)
{
    if (strcmp(str, "while") == 0)
        (*token)->type = WHILE;
    else if (strcmp(str, "until") == 0)
        (*token)->type = UNTIL;
    else if (strcmp(str, "do") == 0)
        (*token)->type = DO;
    else if (strcmp(str, "done") == 0)
        (*token)->type = DONE;
    else if (strcmp(str, "for") == 0)
        (*token)->type = FOR;
    else if (strcmp(str, "in") == 0)
        (*token)->type = IN;
    else
        return 0;

    return 1;
}

static int quotes(struct token **token, char *str)
{
    if (strcmp(str, "'") == 0)
        (*token)->type = S_QUOTE;
    else if (strcmp(str, "\"") == 0)
        (*token)->type = D_QUOTE;
    else
        return 0;

    return 1;
}

static int misc(struct token **token, char *str)
{
    if (is_redir(str) == 1)
        (*token)->type = REDIR;
    else if (strcmp(str, "|") == 0)
        (*token)->type = PIPE;
    else if (strcmp(str, "||") == 0)
        (*token)->type = OR;
    else if (strcmp(str, "&&") == 0)
        (*token)->type = AND;
    else if (strcmp(str, "!") == 0)
        (*token)->type = NEG;
    else
        return 0;

    return 1;
}

static int bracet(struct token **token, char *str)
{

    if (strcmp(str, "(") == 0)
        (*token)->type = L_PAREN;
    else if (strcmp(str, ")") == 0)
        (*token)->type = R_PAREN;
    else if (strcmp(str, "{") == 0)
        (*token)->type = L_BRACE;
    else if (strcmp(str, "}") == 0)
        (*token)->type = R_BRACE;
    else
        return 0;

    return 1;
} 

struct token *create_token(char *str)
{
    if (!str || strcmp(str, "") == 0)
        return NULL;

    struct token *token = malloc(sizeof(struct token));
    token->content = str;

    if (clause_if(&token, str) == 1)
        return token;
    else if (clause_loop(&token, str) == 1)
        return token;
    else if (quotes(&token, str) == 1)
        return token;
    else if (misc(&token, str) == 1)
        return token;
	else if (bracet(&token, str) == 1)
		return token;
    else if (strcmp(str, ";") == 0)
        token->type = SEMICOLON;
    else if (strcmp(str, "\n") == 0)
        token->type = NEWLINE;
    // OTHER
    else if (strchr(str, '=') != NULL)
        token->type = A_WORDS;
    else
        token->type = WORDS;

    return token;
}

// ###################
// #   REDIRECTION   #
// ##################

/**
 * @brief		Check if the string is a redirection argument
 *
 * Determine if the string passed in parameter is used as a redirection
 * argument by using regex.
 *
 * @param str	The string (content of the token)
 *
 * @return		Success or Fail (1 or 0)
 */

static int is_redir_helper(char *str)
{
    return strcmp(str, ">") == 0 || strcmp(str, "<") == 0
        || strcmp(str, ">>") == 0 || strcmp(str, ">&") == 0
        || strcmp(str, "<&") == 0 || strcmp(str, ">|") == 0
        || strcmp(str, "<>") == 0;
}

int is_redir(char *str)
{
    if (!*str)
        return 0;

    if ('0' <= *str && *str <= '9')
    {
        str++;
        return is_redir_helper(str);
    }

    return is_redir_helper(str);
}

// ####################
// #   MISCELLANEOUS   #
// ####################

/**
 * @brief			Mimics the function ungetc but for strings
 *
 * Puts back all characters from a string back into a stream
 *
 * @param str		The string which characters we want to put back
 * @param stream	The stream in which we want to put the characters
 */

void unget_str(const char *str, FILE *stream)
{
    int len = strlen(str);

    for (int i = len - 1; i >= 0; i--)
        ungetc(str[i], stream);
}
