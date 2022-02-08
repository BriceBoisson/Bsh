#ifndef SPACES_H
#define SPACES_H

#include "lexer.h"

/**
 * @brief Remove TOKEN_SPACE tokens from the token list.
 *
 * @param lexer the lexer.
 */
void process_spaces(struct lexer *lexer);

#endif // !SPACES_H
