#ifndef TOKEN_H
#define TOKEN_H

enum types
{
    // IF clause
    IF,
    THEN,
    ELIF,
    ELSE,
    FI,

    // END OF COMMAND
    D_SEMICOLON,
    SEMICOLON,
    NEWLINE,

    // QUOTES
    S_QUOTE,
    D_QUOTE,

    // MISC
    REDIR,
    PIPE,
    AND,
    OR,
    ESC,
    NEG,

    // LOOP clause
    WHILE,
    UNTIL,
    DO,
    DONE,
    FOR,
    IN,

    // OTHER
    WORDS,
    A_WORDS,

    // PARENTHESE ET BRACKET
    L_BRACE,
    R_BRACE,

    L_PAREN,
    R_PAREN,

	// SWITCH
	CASE,
	ESAC,

    // SUBSHELL
    SUBSHELL,

    //switch
    CASE,
    ESAC
};

struct token
{
    enum types type;
    char *content;
};

#endif /* ! TOKEN_H */
