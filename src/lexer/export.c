#include "lexer_tools.h"

void process_export(struct lexer *lexer)
{
    struct lexer_token *token = lexer->tokens;
    while (token && token->next)
    {
        if (token->value && !strcmp(token->value, "export")
            && token->next->type == TOKEN_ASSIGNMENT_WORD)
        {
            token->next->type = TOKEN_WORD;
        }
        token = token->next;
    }
}
