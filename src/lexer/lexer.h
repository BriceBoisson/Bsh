#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum token_type
{
    TOKEN_ERROR,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_ELIF,
    TOKEN_FI,
    TOKEN_THEN,
    TOKEN_DO,
    TOKEN_DONE,
    TOKEN_WHILE,
    TOKEN_UNTIL,
    TOKEN_FOR,
    TOKEN_IN,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_SEMICOLON,
    TOKEN_NEWLINE,
    TOKEN_REDIR,
    TOKEN_IONUMBER,
    TOKEN_PIPE,
    TOKEN_NOT,
    TOKEN_ASSIGNMENT_WORD,
    TOKEN_PARENTHESIS_OPEN,
    TOKEN_SUBSTITUTION_OPEN,
    TOKEN_BACKTICK,
    TOKEN_PARENTHESIS_CLOSE,
    TOKEN_BRACE_OPEN,
    TOKEN_BRACE_CLOSE,
    TOKEN_DOLLAR,
    TOKEN_CASE,
    TOKEN_ESAC,
    TOKEN_WORD,
    TOKEN_WORD_DOUBLE_QUOTE,
    TOKEN_WORD_SINGLE_QUOTE,
    TOKEN_EOF,
    TOKEN_SPACE,
    TOKEN_ALIAS,
    TOKEN_UNALIAS
};

struct lexer_token
{
    enum token_type type;
    char *value;
    struct lexer_token *next;
};

struct lexer_alias
{
    char *name;
    struct lexer_token *value;
    struct lexer_alias *next;
};

/**
 * @brief free an allocated token.
 *
 * @param token the token to free.
 */
struct lexer_token *lexer_token_free(struct lexer_token *token);

/**
 *  @var lexer::input
 *  Member 'input' contains the input string.
 *  @var lexer::tokens
 *  Member 'tokens' contains the head of the list of tokens.
 *  @var lexer::tail
 *  Member 'tail' contains the last token of the list.
 */
struct lexer
{
    char *input;
    struct lexer_token *tokens;
    struct lexer_token *tail;
    struct lexer_token *head;
    bool in_for;
    bool in_variable;
    bool found_for;
    bool found_case;
    struct lexer_token *alias;
    struct lexer_token *alias_prev;
    struct lexer_alias *alias_list;
};

/**
** @brief Allocate and init a new lexer.
** @param input the string to use as input stream.
*/
struct lexer *lexer_create(char *input);

/**
** @brief Fill the token list by creating all the tokens from
** the given string.
**
** @param lexer an empty lexer.
*/
void lexer_build(struct lexer *lexer);

/**
** @brief Return the next token without consume it.
**
** \return the next token from the input stream
** @param lexer the lexer to lex from
*/
struct lexer_token *lexer_peek(struct lexer *lexer);

/**
** @brief Return and consume the next token from the input stream.
**
** \return the next token from the input stream
** @param lexer the lexer to lex from
*/
struct lexer_token *lexer_pop(struct lexer *lexer);

/**
** @brief Append a new token to the token_list of the lexer.
**
** @param lexer the lexer.
** @param token the token to append.
*/
void lexer_append(struct lexer *lexer, struct lexer_token *token);

/**
** @brief Free a lexer, all the tokens and tokens values.
**
** @param lexer the lexer.
*/
void lexer_free(struct lexer *lexer);

/**
 * @brief Set token as head of the token_list.
 *
 * @param lexer the lexer.
 * @param token the new head of the token_list.
 */
void lexer_go_back(struct lexer *lexer, struct lexer_token *token);

/**
 * @brief Print each token in the token_list.
 *
 * @param lexer a lexer.
 */
void lexer_print(struct lexer *lexer);

#endif // !LEXER_H
