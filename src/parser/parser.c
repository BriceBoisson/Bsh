#include "parser.h"

#include <err.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

int evaluate_ast(struct ast *ast);

enum parser_status handle_parser_error(enum parser_status status,
                                       struct ast **res)
{
    ast_free(*res);
    *res = NULL;
    return status;
}

static int display_parser_error(struct ast **res)
{
    warnx("Parser: unexpected token");
    ast_free(*res);
    *res = NULL;
    shell->return_code = 2;
    return 2;
}

struct string_array_with_quotes merge_values(char **values_1, char **values_2,
                                             enum quotes *q_1, enum quotes *q_2)
{
    int length_1 = 0;
    while (values_1 && values_1[length_1] != NULL)
        length_1++;

    int length_2 = 0;
    while (values_2 && values_2[length_2] != NULL)
        length_2++;

    values_1 = realloc(values_1, (length_1 + 1 + length_2) * sizeof(char *));
    q_1 = realloc(q_1, (length_1 + length_2) * sizeof(enum quotes));
    for (int i = length_1; i < length_2 + length_1; i++)
    {
        values_1[i] = values_2[i - length_1];
        q_1[i] = q_2[i - length_1];
    }
    values_1[length_1 + length_2] = NULL;

    struct string_array_with_quotes res = { values_1, q_1 };
    return res;
}

static enum parser_status add_eof_node(struct ast **ast)
{
    struct ast *cur = *ast;
    while (cur && cur->left_child && cur->type == AST_LIST)
    {
        cur = cur->left_child;
    }
    if (!cur || cur->type != AST_LIST)
        return PARSER_ERROR;

    cur->left_child = ast_new(AST_EOF);
    return PARSER_OK;
}

int parse_input(char *input, struct ast **res)
{
    struct lexer *lex = lexer_create(input);
    lexer_build(lex);

    if (shell->exit)
    {
        lexer_free(lex);
        shell->exit = false;
        return 2;
    }

    struct ast *ast = ast_new(AST_LIST);

    // Try EOF
    struct lexer_token *end = lexer_peek(lex);
    if (end->type == TOKEN_EOF || end->type == TOKEN_NEWLINE)
    {
        ast_free(ast);
        lexer_free(lex);
        return PARSER_OK;
    }

    // Try compound_list EOF
    if (parse_compound_list(&ast, lex) == PARSER_OK)
    {
        struct lexer_token *next = lexer_peek(lex);
        if (next->type == TOKEN_EOF || next->type == TOKEN_NEWLINE)
        {
            if (add_eof_node(&ast) == PARSER_ERROR)
                return display_parser_error(&ast);

            if (shell->pretty_print)
                pretty_print(ast);
            if (!res)
            {
                int res_eval = evaluate_ast(ast);
                shell->return_code = res_eval;
                ast_free(ast);
                lexer_free(lex);
            }
            else
            {
                *res = ast;
            }
            return 0;
        }
    }

    lexer_free(lex);
    return display_parser_error(&ast);
}
