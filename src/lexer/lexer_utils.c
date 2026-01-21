#include "lexer_utils.h"

// #############
// #   TOKEN   #
// #############

struct token_map token_lookup[] = {
    // IF
    { "if", IF },
    { "then", THEN },
    { "elif", ELIF },
    { "else", ELSE },
    { "fi", FI },

    // LOOPS
    { "while", WHILE },
    { "until", UNTIL },
    { "for", FOR },
    { "do", DO },
    { "done", DONE },
    { "in", IN },

    // QUOTES
    { "'", S_QUOTE },
    { "\"", D_QUOTE },

    // BRACKETS
    { "(", L_PAREN },
    { ")", R_PAREN },
    { "{", L_BRACE },
    { "}", R_BRACE },

    // OPERATORS
    { "|", PIPE },
    { "||", OR },
    { "&&", AND },

    // MISC
    { "!", NEG },
    { ";", SEMICOLON },
    { "\n", NEWLINE },

    { NULL, 0 }
};

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

struct token *create_token(char *str)
{
    if (!str || strcmp(str, "") == 0)
        return NULL;

    struct token *token = malloc(sizeof(struct token));
    token->content = str;

    if (is_redir(str))
    {
        token->type = REDIR;
        return token;
    }

    // Get the token type for all tokens except WORDS and A_WORDS
    for (int i = 0; token_lookup[i].str != NULL; i++)
    {
        if (strcmp(str, token_lookup[i].str) == 0)
        {
            token->type = token_lookup[i].token_type;
            return token;
        }
    }

    // If we get there then it is either an assignment either a random word
    char *delim = strchr(str, '=');

    // If we have an assign we want to check that the name of the var is valid
    // else all the assign is considered as a whole word
    if (delim != NULL)
    {
        // We mark the end of the var name
        *delim = '\0';

        if (is_valid_name(str) == 1)
            token->type = A_WORDS;
        else
            token->type = WORDS;

        // Restore the string back to original
        *delim = '=';
    }
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
