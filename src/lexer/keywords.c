#include "lexer_tools.h"

bool is_keyword(char *word)
{
    return (
        !strcmp(word, "if") || !strcmp(word, "else") || !strcmp(word, "elif")
        || !strcmp(word, "fi") || !strcmp(word, "then") || !strcmp(word, "!")
        || !strcmp(word, "do") || !strcmp(word, "done") || !strcmp(word, "for")
        || !strcmp(word, "while") || !strcmp(word, "until")
        || !strcmp(word, "case") || !strcmp(word, "esac"));
}

enum token_type get_keyword(char *word)
{
    if (!strcmp(word, "if"))
        return TOKEN_IF;
    if (!strcmp(word, "else"))
        return TOKEN_ELSE;
    if (!strcmp(word, "elif"))
        return TOKEN_ELIF;
    if (!strcmp(word, "fi"))
        return TOKEN_FI;
    if (!strcmp(word, "then"))
        return TOKEN_THEN;
    if (!strcmp(word, "!"))
        return TOKEN_NOT;
    if (!strcmp(word, "do"))
        return TOKEN_DO;
    if (!strcmp(word, "done"))
        return TOKEN_DONE;
    if (!strcmp(word, "for"))
        return TOKEN_FOR;
    if (!strcmp(word, "while"))
        return TOKEN_WHILE;
    if (!strcmp(word, "until"))
        return TOKEN_UNTIL;
    if (!strcmp(word, "case"))
        return TOKEN_CASE;
    if (!strcmp(word, "esac"))
        return TOKEN_ESAC;
    return TOKEN_ERROR;
}
