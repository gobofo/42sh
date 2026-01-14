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
    A_WORDS
};

struct token
{
    enum types type;
    char *content;
};

#endif /* ! TOKEN_H */
