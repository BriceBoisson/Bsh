#include "lexer_tools.h"

void create_and_append_token(struct lexer *lexer, enum token_type type,
                             char *value)
{
    struct lexer_token *token = calloc(1, sizeof(struct lexer_token));
    token->type = type;
    token->value = value;
    lexer_append(lexer, token);
}
