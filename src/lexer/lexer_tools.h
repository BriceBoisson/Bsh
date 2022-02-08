#ifndef LEXER_TOOLS_H
#define LEXER_TOOLS_H

#include "lexer.h"
#include "spaces.h"

void create_and_append_token(struct lexer *lexer, enum token_type type,
                             char *value);

char **split_in_words(char *input);

enum token_type get_keyword(char *word);

bool is_keyword(char *word);

bool is_int(char *word);

void process_export(struct lexer *lexer);

void process_alias(struct lexer_token *prev, struct lexer_token *head,
                   struct lexer *lexer);

struct lexer_alias *get_alias(char *name);

void lexer_append_alias(struct lexer *lexer, struct lexer_alias *alias);

void process_unalias(struct lexer_token *prev, struct lexer_token *head,
                     struct lexer *lexer);

#endif // !LEXER_TOOLS_H
