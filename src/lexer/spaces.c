#include "spaces.h"

static bool is_word(enum token_type type)
{
    int res =
        type >= TOKEN_WORD_DOUBLE_QUOTE && type <= TOKEN_WORD_SINGLE_QUOTE;
    return res;
}

void process_spaces(struct lexer *lexer)
{
    struct lexer_token *token = lexer->tokens;
    while (token
           && (token->type == TOKEN_SPACE || token->type == TOKEN_NEWLINE))
    {
        struct lexer_token *next = token->next;
        lexer_token_free(token);
        token = next;
        lexer->tokens = token;
    }
    while (token && token->type != TOKEN_EOF)
    {
        struct lexer_token *next = token->next;
        if (!next)
        {
            token = next;
            continue;
        }
        if (next->type == TOKEN_SPACE)
        {
            while (next->type == TOKEN_SPACE)
            {
                struct lexer_token *tmp = next->next;
                token->next = next->next;
                free(next->value);
                free(next);
                next = tmp;
            }
        }
        else if (is_word(token->type) && is_word(next->type)
                 && next->type == token->type)
        {
            token->next = next->next;
            token->value = realloc(
                token->value, strlen(token->value) + strlen(next->value) + 2);
            strcat(token->value, next->value);
            free(next->value);
            free(next);
        }
        token = token->next;
    }
}
