#ifndef TOKEN_H
#define TOKEN_H

enum types
{
    IF,
    THEN,
    ELIF,
    ELSE,
    FI,
    SEMICOLON,
    NEWLINE,
    S_QUOTE,
    WORDS
};

struct token
{
    enum types type;
    char *content;
};

#endif /* ! TOKEN_H */
