#include "lexer.h"
#include "lexer_tools.h"

bool is_int(char *input)
{
    int i = 0;
    while (input[i])
    {
        if (input[i] < '0' || input[i] > '9')
            return false;
        i++;
    }
    return true;
}

// void words_to_ionumber(struct lexer *lexer)
// {
//     struct lexer_token *token = lexer->tokens;
//     struct lexer_token *prev = lexer->tokens;
//     while (token)
//     {
//         if (token->type == TOKEN_REDIR && prev && prev->type == TOKEN_WORD
//             && is_int(prev->value))
//         {
//             prev->type = TOKEN_IONUMBER;
//         }
//         prev = token;
//         token = token->next;
//     }
// }
