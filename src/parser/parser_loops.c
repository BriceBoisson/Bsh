#include "parser.h"

enum parser_status parse_rule_while(struct ast **ast, struct lexer *lexer)
{
    struct lexer_token *tok = lexer_peek(lexer);

    // Try While
    if (tok->type != TOKEN_WHILE)
        return handle_parser_error(PARSER_ERROR, ast);
    lexer_pop(lexer); // token While

    // Try compound_list
    *ast = ast_new(AST_WHILE);
    struct ast *ast_condition = ast_new(AST_LIST);
    enum parser_status status_compound_list =
        parse_compound_list(&ast_condition, lexer);
    // If status is ERROR, assignment is still legal to free everything
    (*ast)->condition = ast_condition;
    if (status_compound_list == PARSER_ERROR)
        return handle_parser_error(status_compound_list, ast);

    // Try do_group
    struct ast *ast_do = NULL;
    enum parser_status status_do_group = parse_do_group(&ast_do, lexer);
    (*ast)->left_child = ast_do;
    if (status_do_group == PARSER_ERROR)
        return handle_parser_error(status_do_group, ast);

    return PARSER_OK;
}

enum parser_status parse_rule_until(struct ast **ast, struct lexer *lexer)
{
    struct lexer_token *tok = lexer_peek(lexer);

    // Try Until
    if (tok->type != TOKEN_UNTIL)
        return handle_parser_error(PARSER_ERROR, ast);
    lexer_pop(lexer); // token Until

    // Try compound_list
    *ast = ast_new(AST_UNTIL);
    struct ast *ast_condition = ast_new(AST_LIST);
    enum parser_status status_compound_list =
        parse_compound_list(&ast_condition, lexer);
    // If status is ERROR, assignment is still legal to free everything
    (*ast)->condition = ast_condition;
    if (status_compound_list == PARSER_ERROR)
        return handle_parser_error(status_compound_list, ast);

    // Try do_group
    struct ast *ast_do = NULL;
    enum parser_status status_do_group = parse_do_group(&ast_do, lexer);
    (*ast)->left_child = ast_do;
    if (status_do_group == PARSER_ERROR)
        return handle_parser_error(status_do_group, ast);

    return PARSER_OK;
}

enum parser_status parse_do_group(struct ast **ast, struct lexer *lexer)
{
    struct lexer_token *tok = lexer_peek(lexer);

    // Try Do
    if (tok->type != TOKEN_DO)
        return handle_parser_error(PARSER_ERROR, ast);
    lexer_pop(lexer); // token While

    // Try compound_list
    struct ast *ast_body = ast_new(AST_LIST);
    enum parser_status status_compound_list =
        parse_compound_list(&ast_body, lexer);

    *ast = ast_body;
    if (status_compound_list == PARSER_ERROR)
        return handle_parser_error(status_compound_list, ast);

    // Try Done
    tok = lexer_peek(lexer);
    if (tok->type != TOKEN_DONE)
        return handle_parser_error(PARSER_ERROR, ast);
    lexer_pop(lexer); // token Done

    return PARSER_OK;
}

enum parser_status parse_for(struct ast **ast, struct lexer *lexer)
{
    struct lexer_token *tok = lexer_peek(lexer);

    // Try For
    if (tok->type != TOKEN_FOR)
        return handle_parser_error(PARSER_ERROR, ast);
    lexer_pop(lexer); // token For

    *ast = ast_new(AST_FOR);
    char **values = NULL;
    enum quotes *enclosure = NULL;
    size_t len = 0;

    // Try WORD
    tok = lexer_peek(lexer);
    if (tok->type == TOKEN_WORD || tok->type == TOKEN_WORD_DOUBLE_QUOTE
        || tok->type == TOKEN_WORD_SINGLE_QUOTE)
    {
        len++;
        values = realloc(values, (len + 1) * sizeof(char *));
        values[len - 1] = tok->value;
        values[len] = NULL;
        enclosure = realloc(enclosure, len * sizeof(enum quotes));
        enclosure[len - 1] = tok->type - TOKEN_WORD;

        (*ast)->value = values;
        (*ast)->enclosure = enclosure;

        lexer_pop(lexer); // token WORD
    }
    else
        return handle_parser_error(PARSER_ERROR, ast);

    // Try [';']
    tok = lexer_peek(lexer);
    bool semi_colon = false;
    if (tok->type == TOKEN_SEMICOLON)
    {
        semi_colon = true;
        lexer_pop(lexer); // token ';'
    }

    // Try ('\n')*
    while ((tok = lexer_peek(lexer))->type == TOKEN_NEWLINE)
        lexer_pop(lexer); // token '\n'

    // Try in from [('\n')* 'in' (WORD)* (';'|'\n')]
    bool in = false;
    if (tok->type == TOKEN_IN)
    {
        in = true;
        lexer_pop(lexer); // token in
        //? add 'in' in char **values
        len++;
        values = realloc(values, (len + 1) * sizeof(char *));
        values[len - 1] = "in";
        values[len] = NULL;
        enclosure = realloc(enclosure, len * sizeof(enum quotes));
        enclosure[len - 1] = 0;

        (*ast)->value = values;
        (*ast)->enclosure = enclosure;

        //? already took left part of \ condition
        if (semi_colon)
            return handle_parser_error(PARSER_ERROR, ast);

        //? try rest of condition
        // Try WORD*
        tok = lexer_peek(lexer);
        while (tok->type == TOKEN_WORD || tok->type == TOKEN_WORD_DOUBLE_QUOTE
               || tok->type == TOKEN_WORD_SINGLE_QUOTE)
        {
            len++;
            values = realloc(values, (len + 1) * sizeof(char *));
            values[len - 1] = tok->value;
            values[len] = NULL;
            enclosure = realloc(enclosure, len * sizeof(enum quotes));
            enclosure[len - 1] = tok->type - TOKEN_WORD;

            (*ast)->value = values;
            (*ast)->enclosure = enclosure;

            lexer_pop(lexer); // token WORD
            tok = lexer_peek(lexer);
        }

        if (tok->type == TOKEN_SEMICOLON)
            lexer_pop(lexer); // token ';'
        else if (tok->type == TOKEN_NEWLINE)
            lexer_pop(lexer); // token '\n'
        else
            return handle_parser_error(PARSER_ERROR, ast);
    }

    if (in)
    {
        // Try ('\n')*
        while ((tok = lexer_peek(lexer))->type == TOKEN_NEWLINE)
            lexer_pop(lexer); // token '\n'
    }

    // Try do_group
    struct ast *ast_do = NULL;
    enum parser_status status_do_group = parse_do_group(&ast_do, lexer);
    (*ast)->left_child = ast_do;
    if (status_do_group == PARSER_ERROR)
        return handle_parser_error(status_do_group, ast);

    (*ast)->left_child = ast_do;

    return PARSER_OK;
}
