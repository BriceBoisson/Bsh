#include "lexer.h"

static char *get_token_string(enum token_type type)
{
    char *token_string[] = { "ERROR",
                             "IF",
                             "ELSE",
                             "ELIF",
                             "FI",
                             "THEN",
                             "DO",
                             "DONE",
                             "WHILE",
                             "UNTIL",
                             "FOR",
                             "IN",
                             "&&",
                             "||",
                             "SEMICOLON",
                             "NEWLINE",
                             "REDIR",
                             "IONUMBER",
                             "PIPE",
                             "NOT",
                             "ASSIGNMENT_WORD",
                             "PARENTHESIS_OPEN",
                             "SUBSTITUTION",
                             "BACKTICK",
                             "PARENTHESIS_CLOSE",
                             "BRACE_OPEN",
                             "BRACE_CLOSE",
                             "DOLLAR",
                             "CASE",
                             "ESAC",
                             "WORD",
                             "WORD_DOUBLE_QUOTE",
                             "WORD_SINGLE_QUOTE",
                             "EOF",
                             "SPACE" };
    return token_string[type];
}

void lexer_print(struct lexer *lexer)
{
    printf("lexer output: ");
    struct lexer_token *token = lexer->tokens;
    while (token)
    {
        if (token->type != TOKEN_REDIR)
            printf("%s ", get_token_string(token->type));
        else
            printf("%s ", token->value);
        token = token->next;
    }
    printf("\n");
}
