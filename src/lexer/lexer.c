#include "lexer.h"

#include "../bsh.h"
#include "lexer_tools.h"

extern struct shell *shell;

struct lexer_token *lexer_token_free(struct lexer_token *token)
{
    free(token->value);
    free(token);
    return NULL;
}

struct lexer *lexer_create(char *input)
{
    struct lexer *lexer = calloc(1, sizeof(struct lexer));
    lexer->input = input;
    lexer->tail = NULL;
    lexer->head = NULL;
    lexer->tokens = NULL;
    return lexer;
}

struct lexer_token *lexer_peek(struct lexer *lexer)
{
    return lexer->head;
}

struct lexer_token *lexer_pop(struct lexer *lexer)
{
    struct lexer_token *token = lexer->head;
    lexer->head = lexer->head->next;
    return token;
}

void lexer_append(struct lexer *lexer, struct lexer_token *token)
{
    token->next = NULL;
    if (lexer->tail)
    {
        lexer->tail->next = token;
        lexer->tail = token;
    }
    else
    {
        lexer->tokens = token;
        lexer->tail = token;
    }
}

void lexer_free(struct lexer *lexer)
{
    struct lexer_token *token = lexer->tokens;
    while (token)
    {
        struct lexer_token *next = token->next;
        lexer_token_free(token);
        token = next;
    }
    struct lexer_alias *alias = lexer->alias_list;
    while (alias)
    {
        struct lexer_alias *next = alias->next;
        free(alias->name);
        struct lexer_token *token = alias->value;
        while (token)
        {
            struct lexer_token *next = token->next;
            lexer_token_free(token);
            token = next;
        }
        free(alias);
        alias = next;
    }
    lexer->alias_list = NULL;
    lexer->head = NULL;
    lexer->tail = NULL;
    free(lexer);
}

static bool is_separator(char c)
{
    return (c == ';' || c == '\n');
}

static enum token_type get_separator(char c)
{
    if (c == ';')
        return TOKEN_SEMICOLON;
    if (c == '\n')
        return TOKEN_NEWLINE;
    return TOKEN_ERROR;
}

static bool is_quote(char c)
{
    return (c == '\'' || c == '\"' || c == '`');
}

static enum token_type get_quote(char c)
{
    if (c == '\'')
        return TOKEN_WORD_SINGLE_QUOTE;
    if (c == '\"')
        return TOKEN_WORD_DOUBLE_QUOTE;
    if (c == '`')
        return TOKEN_BACKTICK;
    return TOKEN_ERROR;
}

static void create_word_and_append(char *word, int word_pos, bool *in_cmd,
                                   struct lexer *lexer,
                                   enum token_type *word_type)
{
    if (!word)
        return;
    word[word_pos] = 0;
    struct lexer_alias *alias = get_alias(word);
    if (alias && !lexer->alias)
    {
        lexer_append_alias(lexer, alias);
        free(word);
        return;
    }
    if (*word_type == TOKEN_WORD
        && (!strcmp(word, "alias") || !strcmp(word, "unalias")))
    {
        lexer->alias_prev = lexer->tail;
        create_and_append_token(
            lexer, !strcmp(word, "alias") ? TOKEN_ALIAS : TOKEN_UNALIAS, NULL);
        lexer->alias = lexer->tail;
        free(word);
        *word_type = TOKEN_WORD;
        return;
    }
    if (*word_type == TOKEN_WORD && (!strcmp(word, "in"))
        && ((!lexer->in_for && lexer->found_for) || lexer->found_case))
    {
        create_and_append_token(lexer, TOKEN_IN, NULL);
        if (lexer->found_for)
            lexer->in_for = true;
        free(word);
        return;
    }
    struct lexer_token *token = calloc(1, sizeof(struct lexer_token));
    token->type = is_keyword(word) && !lexer->alias
            && (!(*in_cmd) || lexer->found_case
                || (lexer->found_for && !strcmp(word, "do")))
        ? get_keyword(word)
        : *word_type;
    if (token->type >= TOKEN_WORD && !lexer->found_case)
        *in_cmd = true;
    if (token->type == TOKEN_FOR)
        lexer->found_for = true;
    if (token->type == TOKEN_CASE)
        lexer->found_case = true;
    if (token->type == TOKEN_ESAC)
        lexer->found_case = false;
    token->value = word;
    word = NULL;
    word_pos = 0;
    lexer_append(lexer, token);
}

static bool is_pipe(char c, char next)
{
    return (c == '|' && next != '|');
}

static bool is_redir(char c1)
{
    return (c1 == '<' || c1 == '>');
}

static char *get_redir(char c1, char c2)
{
    char *res = calloc(3, sizeof(char));
    if (c1 == '<')
    {
        res[0] = '<';
        if (c2 == '&' || c2 == '>')
            res[1] = c2;
    }
    if (c1 == '>')
    {
        res[0] = '>';
        if (c2 == '&' || c2 == '>' || c2 == '|')
            res[1] = c2;
    }
    return res;
}

static bool is_special(char c)
{
    return (c == '(' || c == ')' || c == '{' || c == '}' || c == '$');
}

static enum token_type get_special(char c)
{
    if (c == '(')
        return TOKEN_PARENTHESIS_OPEN;
    if (c == ')')
        return TOKEN_PARENTHESIS_CLOSE;
    if (c == '{')
        return TOKEN_BRACE_OPEN;
    if (c == '}')
        return TOKEN_BRACE_CLOSE;
    if (c == '$')
        return TOKEN_DOLLAR;
    return TOKEN_ERROR;
}

static bool is_word_alphanum(char *word, int len)
{
    for (int i = 0; i < len; i++)
        if (!((word[i] >= 'a' && word[i] <= 'z')
              || (word[i] >= 'A' && word[i] <= 'Z')
              || (word[i] >= '0' && word[i] <= '9') || word[i] == '_'))
            return false;
    return true;
}

static void word_lexer(struct lexer *lexer, char *input, bool *in_cmd,
                       enum token_type *word_type)
{
    int j = 0;
    char *word = NULL;
    int word_pos = 0;
    while (input[j])
    {
        if (input[j] == '\\')
        {
            word = realloc(word, (word_pos + 3) * sizeof(char));
            word[word_pos++] = input[j++];
            if (input[j] == 0)
                break;
            word[word_pos++] = input[j++];
            if (input[j] == 0)
                break;
        }
        if ((*word_type == TOKEN_WORD && is_separator(input[j]))
            || (is_pipe(input[j], input[j + 1]) && *word_type == TOKEN_WORD))
        {
            if (word)
            {
                create_word_and_append(word, word_pos, in_cmd, lexer,
                                       word_type);
                word = NULL;
                word_pos = 0;
            }
            create_and_append_token(
                lexer,
                is_separator(input[j]) ? get_separator(input[j]) : TOKEN_PIPE,
                NULL);
            if (is_separator(input[j]))
            {
                if (lexer->alias != NULL && lexer->alias->next != lexer->tail)
                {
                    if (lexer->alias->type == TOKEN_ALIAS)
                        process_alias(lexer->alias_prev, lexer->alias, lexer);
                    else
                        process_unalias(lexer->alias_prev, lexer->alias, lexer);
                }
                else if (lexer->alias)
                {
                    if (lexer->alias_prev)
                    {
                        lexer_token_free(lexer->alias_prev->next);
                        lexer->alias_prev->next = lexer->tail;
                    }
                    else
                    {
                        lexer_token_free(lexer->alias);
                        lexer->tokens = lexer->tail;
                    }
                }
                if (input[j] == '\n')
                {
                    struct lexer_alias *alias = lexer->alias_list;
                    while (alias)
                    {
                        struct lexer_alias *next = alias->next;
                        alias->next = shell->alias_list;
                        shell->alias_list = alias;
                        alias = next;
                    }
                    lexer->alias_list = NULL;
                }
                lexer->alias = NULL;
                lexer->in_for = false;
                lexer->found_for = false;
            }
            *in_cmd = false;
        }
        else if (*word_type == TOKEN_WORD
                 && ((input[j] == '&' && input[j + 1] == '&')
                     || (input[j] == '|' && input[j + 1] == '|')))
        {
            if (word)
            {
                create_word_and_append(word, word_pos, in_cmd, lexer,
                                       word_type);
                word = NULL;
                word_pos = 0;
            }
            create_and_append_token(
                lexer, input[j] == '&' ? TOKEN_AND : TOKEN_OR, NULL);
            j++;
        }
        else if (*word_type == TOKEN_WORD && is_special(input[j]))
        {
            if (input[j] == '}' && lexer->in_variable)
            {
                word = realloc(word, (word_pos + 2) * sizeof(char));
                word[word_pos++] = input[j];
                lexer->in_variable = false;
            }
            else
            {
                if (word && (input[j] != '$' || input[j + 1] == '('))
                {
                    create_word_and_append(word, word_pos, in_cmd, lexer,
                                           word_type);
                    word = NULL;
                    word_pos = 0;
                }
                if (input[j] == '$')
                {
                    if (input[j + 1] == '(')
                    {
                        *in_cmd = false;
                        j++;
                        create_and_append_token(lexer, TOKEN_SUBSTITUTION_OPEN,
                                                NULL);
                    }
                    else
                    {
                        word = realloc(word, (word_pos + 3) * sizeof(char));
                        word[word_pos++] = input[j];
                        if (input[j + 1] == '{' || input[j + 1] == '$')
                        {
                            word[word_pos++] = input[++j];
                            lexer->in_variable = true;
                        }
                    }
                }
                else
                {
                    if (input[j] == '{' || input[j] == '(')
                        *in_cmd = false;
                    create_and_append_token(lexer, get_special(input[j]), NULL);
                }
            }
        }
        else if (*word_type == TOKEN_WORD && is_redir(input[j]))
        {
            if (word)
            {
                word[word_pos] = 0;
                if (is_int(word))
                {
                    create_and_append_token(lexer, TOKEN_IONUMBER, word);
                }
                else
                    create_word_and_append(word, word_pos, in_cmd, lexer,
                                           word_type);
                word = NULL;
                word_pos = 0;
            }
            create_and_append_token(lexer, TOKEN_REDIR,
                                    get_redir(input[j], input[j + 1]));
            if (input[j + 1] != 0)
                j++;
        }
        else if (*word_type == TOKEN_WORD && input[j] == '='
                 && (!lexer->tail || lexer->tail->type != TOKEN_ASSIGNMENT_WORD)
                 && is_word_alphanum(word, word_pos))
        {
            if (word)
            {
                create_word_and_append(word, word_pos, in_cmd, lexer,
                                       word_type);
                word = NULL;
                word_pos = 0;
                lexer->tail->type = TOKEN_ASSIGNMENT_WORD;
            }
        }
        else if (is_quote(input[j])
                 && (*word_type == get_quote(input[j])
                     || *word_type == TOKEN_WORD))
        {
            if (word)
            {
                create_word_and_append(word, word_pos, in_cmd, lexer,
                                       word_type);
                word = NULL;
                word_pos = 0;
            }
            if (lexer->alias)
            {
                j++;
                continue;
            }
            if (*word_type == TOKEN_WORD && input[j] != '`')
                *word_type = get_quote(input[j]);
            else if (*word_type == TOKEN_WORD && input[j] == '`')
            {
                create_and_append_token(lexer, TOKEN_BACKTICK, NULL);
            }
            else if (get_quote(input[j]) == *word_type)
            {
                *word_type = TOKEN_WORD;
            }
        }
        else
        {
            word = realloc(word, (word_pos + 2) * sizeof(char));
            word[word_pos++] = input[j];
        }
        j++;
    }
    if (word)
    {
        create_word_and_append(word, word_pos, in_cmd, lexer, word_type);
        word = NULL;
        word_pos = 0;
    }
    free(input);
}

void lexer_build(struct lexer *lexer)
{
    bool in_cmd = false;
    char **words = split_in_words(lexer->input);
    enum token_type word_type = TOKEN_WORD;
    for (int i = 0; words[i]; i++)
    {
        word_lexer(lexer, words[i], &in_cmd, &word_type);
        create_and_append_token(lexer, TOKEN_SPACE, NULL);
    }
    if (word_type != TOKEN_WORD)
    {
        fprintf(stderr, "Error: quote <%c> is not terminated.\n",
                word_type == TOKEN_WORD_SINGLE_QUOTE ? '\'' : '\"');
        shell->return_code = 2;
        shell->exit = true;
    }
    create_and_append_token(lexer, TOKEN_EOF, NULL);
    process_spaces(lexer);
    process_export(lexer);
    if (shell->verbose)
        lexer_print(lexer);
    free(words);
    lexer->head = lexer->tokens;
}

void lexer_go_back(struct lexer *lexer, struct lexer_token *token)
{
    lexer->head = token;
}
