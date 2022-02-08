#include "parser.h"

enum parser_status parse_rule_if(struct ast **ast, struct lexer *lexer)
{
    // Check If
    struct lexer_token *tok = lexer_peek(lexer);
    if (tok->type != TOKEN_IF)
        return handle_parser_error(PARSER_ERROR, ast);
    lexer_pop(lexer); // token IF

    *ast = ast_new(AST_IF);

    // Check compound_list (condition)
    struct ast *ast_condition = ast_new(AST_LIST);
    enum parser_status status_compound_list =
        parse_compound_list(&ast_condition, lexer);
    // If status is ERROR, assignment is still legal to free everything
    (*ast)->condition = ast_condition;
    if (status_compound_list == PARSER_ERROR)
        return handle_parser_error(status_compound_list, ast);

    // Check Then
    tok = lexer_peek(lexer);
    if (tok->type != TOKEN_THEN)
        return handle_parser_error(PARSER_ERROR, ast);
    lexer_pop(lexer); // token THEN

    // Check compound_list (true block)
    struct ast *ast_true_block = ast_new(AST_LIST);
    status_compound_list = parse_compound_list(&ast_true_block, lexer);
    (*ast)->left_child = ast_true_block;
    if (status_compound_list == PARSER_ERROR)
        return handle_parser_error(status_compound_list, ast);

    // Check First(else_clause) = {Else, Elif}
    tok = lexer_peek(lexer);
    if (tok->type == TOKEN_ELSE || tok->type == TOKEN_ELIF)
    {
        // Check else_clause
        struct ast *ast_false_block = NULL;
        status_compound_list = parse_else_clause(&ast_false_block, lexer);
        (*ast)->right_child = ast_false_block;
        if (status_compound_list == PARSER_ERROR)
            return handle_parser_error(status_compound_list, ast);
    }

    // Check Fi
    tok = lexer_peek(lexer);
    if (tok->type != TOKEN_FI)
        return handle_parser_error(PARSER_ERROR, ast);
    lexer_pop(lexer); // token FI

    return PARSER_OK;
}

enum parser_status parse_else_clause(struct ast **ast, struct lexer *lexer)
{
    // Double check First(else_clause), should always be correct but safety
    // first
    struct lexer_token *tok = lexer_peek(lexer);
    enum parser_status status = PARSER_OK;
    if (tok->type == TOKEN_ELSE)
    {
        lexer_pop(lexer); // token ELSE

        // Check compound_list
        *ast = ast_new(AST_LIST);
        status = parse_compound_list(ast, lexer);
        if (status == PARSER_ERROR)
            return handle_parser_error(status, ast);
    }
    else if (tok->type == TOKEN_ELIF)
    {
        lexer_pop(lexer); // token ELIF
        (*ast) = ast_new(AST_IF);

        // Check compound_list
        struct ast *ast_elif_condition = ast_new(AST_LIST);
        status = parse_compound_list(&ast_elif_condition, lexer);
        (*ast)->condition = ast_elif_condition;
        if (status == PARSER_ERROR)
            return handle_parser_error(status, ast);

        // Check Then
        tok = lexer_peek(lexer);
        if (tok->type != TOKEN_THEN)
            return handle_parser_error(PARSER_ERROR, ast);
        lexer_pop(lexer); // token THEN

        // Check compound_list (true block)
        struct ast *ast_true_block = ast_new(AST_LIST);
        status = parse_compound_list(&ast_true_block, lexer);
        (*ast)->left_child = ast_true_block;
        if (status == PARSER_ERROR)
            return handle_parser_error(status, ast);

        // Check First(else_clause) = {Else, Elif}
        tok = lexer_peek(lexer);
        if (tok->type == TOKEN_ELSE || tok->type == TOKEN_ELIF)
        {
            struct ast *ast_false_block = NULL;
            status = parse_else_clause(&ast_false_block, lexer);
            (*ast)->right_child = ast_false_block;
            if (status == PARSER_ERROR)
                return handle_parser_error(status, ast);
        }
    }
    else
        return handle_parser_error(PARSER_ERROR, ast);

    return PARSER_OK;
}

enum parser_status parse_rule_case(struct ast **ast, struct lexer *lexer)
{
    struct lexer_token *tok = lexer_peek(lexer);

    // Try Case
    if (tok->type != TOKEN_CASE)
        return handle_parser_error(PARSER_ERROR, ast);
    lexer_pop(lexer); // token Case

    // Try WORD
    tok = lexer_peek(lexer);
    if (tok->type != TOKEN_WORD && tok->type != TOKEN_WORD_DOUBLE_QUOTE
        && tok->type != TOKEN_WORD_SINGLE_QUOTE)
        return handle_parser_error(PARSER_ERROR, ast);
    char **val = calloc(2, sizeof(char *));
    enum quotes *enclosure = calloc(1, sizeof(enum quotes));
    enclosure[0] = tok->type - TOKEN_WORD;
    val[0] = tok->value;
    lexer_pop(lexer); // token WORD

    *ast = ast_new(AST_CASE);
    (*ast)->value = val;
    (*ast)->enclosure = enclosure;

    // Try ('\n')*
    while ((tok = lexer_peek(lexer))->type == TOKEN_NEWLINE)
        lexer_pop(lexer); // token \n

    // Try in
    tok = lexer_peek(lexer);
    if (tok->type != TOKEN_IN)
        return handle_parser_error(PARSER_ERROR, ast);
    lexer_pop(lexer); // token in

    // Try ('\n')*
    while ((tok = lexer_peek(lexer))->type == TOKEN_NEWLINE)
        lexer_pop(lexer); // token \n

    // Check First(case_clause) = First(case_item) = {(, WORD}
    tok = lexer_peek(lexer);
    if (tok->type == TOKEN_PARENTHESIS_OPEN || tok->type == TOKEN_WORD
        || tok->type == TOKEN_WORD_SINGLE_QUOTE
        || tok->type == TOKEN_WORD_DOUBLE_QUOTE)
    {
        // Check case_clause
        struct ast *ast_content_block = NULL;
        enum parser_status status_case_clause =
            parse_case_clause(&ast_content_block, lexer);
        (*ast)->left_child = ast_content_block;
        if (status_case_clause == PARSER_ERROR)
            return handle_parser_error(status_case_clause, ast);
    }

    // Try Esac
    tok = lexer_peek(lexer);
    if (tok->type != TOKEN_ESAC)
        return handle_parser_error(PARSER_ERROR, ast);
    lexer_pop(lexer); // token Esac

    return PARSER_OK;
}

enum parser_status parse_case_clause(struct ast **ast, struct lexer *lexer)
{
    struct lexer_token *tok = lexer_peek(lexer);

    // Try case_item
    enum parser_status status_case_item = parse_case_item(ast, lexer);
    if (status_case_item == PARSER_ERROR)
        return handle_parser_error(status_case_item, ast);

    // Try (';;' ('\n')* case_item)*
    struct ast *cur_item = *ast;
    struct lexer_token *save_tok = lexer_peek(lexer);
    while (true)
    {
        tok = lexer_peek(lexer);
        save_tok = tok;

        // Try ;
        if (tok->type != TOKEN_SEMICOLON)
            break;
        lexer_pop(lexer); // token ;

        // Try ;
        tok = lexer_peek(lexer);
        if (tok->type == TOKEN_SEMICOLON)
            lexer_pop(lexer); // token ;

        // Try ('\n')*
        while ((tok = lexer_peek(lexer))->type == TOKEN_NEWLINE)
            lexer_pop(lexer); // token \n

        // Try case_item
        enum parser_status status_case_item =
            parse_case_item(&(cur_item->right_child), lexer);
        if (status_case_item == PARSER_ERROR)
        {
            lexer_go_back(lexer, save_tok);
            break;
        }
        cur_item = cur_item->right_child;
    }

    tok = lexer_peek(lexer);
    // Try ;;
    if (tok->type == TOKEN_SEMICOLON)
    {
        lexer_pop(lexer); // token ;
        tok = lexer_peek(lexer);
        if (tok->type == TOKEN_SEMICOLON)
            lexer_pop(lexer); // token ;
    }

    // Try ('\n')*
    while ((tok = lexer_peek(lexer))->type == TOKEN_NEWLINE)
        lexer_pop(lexer); // token \n

    return PARSER_OK;
}

enum parser_status parse_case_item(struct ast **ast, struct lexer *lexer)
{
    struct lexer_token *tok = lexer_peek(lexer);

    // Try [(]
    if (tok->type == TOKEN_PARENTHESIS_OPEN)
        lexer_pop(lexer); // token (

    // Try WORD
    tok = lexer_peek(lexer);
    if (tok->type != TOKEN_WORD && tok->type != TOKEN_WORD_SINGLE_QUOTE
        && tok->type != TOKEN_WORD_DOUBLE_QUOTE)
        return handle_parser_error(PARSER_ERROR, ast);

    *ast = ast_new(AST_CASE_SWITCH);
    (*ast)->value = calloc(2, sizeof(char *));
    (*ast)->value[0] = tok->value;
    (*ast)->enclosure = calloc(1, sizeof(enum quotes));
    (*ast)->enclosure[0] = tok->type - TOKEN_WORD;
    size_t val_len = 1;
    lexer_pop(lexer); // token WORD

    // Try (| WORD)*
    while (true)
    {
        // Try |
        tok = lexer_peek(lexer);
        if (tok->type != TOKEN_PIPE)
            break;
        lexer_pop(lexer); // token |

        // Try WORD
        tok = lexer_peek(lexer);
        if (tok->type != TOKEN_WORD && tok->type != TOKEN_WORD_SINGLE_QUOTE
            && tok->type != TOKEN_WORD_DOUBLE_QUOTE)
            return handle_parser_error(PARSER_ERROR, ast);

        val_len++;
        (*ast)->value = realloc((*ast)->value, (val_len + 1) * sizeof(char *));
        (*ast)->value[val_len - 1] = tok->value;
        (*ast)->value[val_len] = NULL;
        (*ast)->enclosure =
            realloc((*ast)->enclosure, val_len * sizeof(char *));
        (*ast)->enclosure[val_len - 1] = tok->type - TOKEN_WORD;
        lexer_pop(lexer); // token WORD
    }

    // Try )
    if (tok->type != TOKEN_PARENTHESIS_CLOSE)
        return handle_parser_error(PARSER_ERROR, ast);
    lexer_pop(lexer); // token )

    // Try ('\n')*
    while ((tok = lexer_peek(lexer))->type == TOKEN_NEWLINE)
        lexer_pop(lexer); // token \n

    // Try [compound_list]
    struct lexer_token *save_tok = lexer_peek(lexer);
    struct ast *ast_compound_list = ast_new(AST_LIST);
    enum parser_status status_compound_list =
        parse_compound_list(&ast_compound_list, lexer);
    if (status_compound_list == PARSER_ERROR)
    {
        ast_free(ast_compound_list);
        lexer_go_back(lexer, save_tok);
    }
    else
    {
        (*ast)->left_child = ast_compound_list;
    }
    return PARSER_OK;
}
