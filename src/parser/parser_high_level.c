#include "parser.h"

enum parser_status parse_compound_list(struct ast **ast, struct lexer *lexer)
{
    struct lexer_token *tok = lexer_peek(lexer);

    // Try ('\n')*
    while ((tok = lexer_peek(lexer))->type == TOKEN_NEWLINE)
        lexer_pop(lexer); // token \n

    // Try and_or
    enum parser_status status_and_or =
        parse_and_or(&(*ast)->right_child, lexer);
    if (status_and_or == PARSER_ERROR)
        return handle_parser_error(status_and_or, ast);

    // Try (';' and_or)*
    struct ast *cur_list_node = *ast;
    while (true)
    {
        struct lexer_token *save_tok = lexer_peek(lexer);
        if (save_tok->type != TOKEN_SEMICOLON
            && save_tok->type != TOKEN_NEWLINE)
            break;
        lexer_pop(lexer);

        // Try ('\n')*
        while ((tok = lexer_peek(lexer))->type == TOKEN_NEWLINE)
            lexer_pop(lexer); // token \n

        struct ast *new_list = ast_new(AST_LIST);
        enum parser_status status = parse_and_or(&new_list->right_child, lexer);
        if (status == PARSER_ERROR)
        {
            ast_free(new_list);
            lexer_go_back(lexer, save_tok);
            break;
        }

        cur_list_node->left_child = new_list;
        cur_list_node = cur_list_node->left_child;
    }

    // Try [';'] and skip it if present
    tok = lexer_peek(lexer);
    if (tok->type == TOKEN_SEMICOLON || tok->type == TOKEN_NEWLINE)
    {
        lexer_pop(lexer);
        // Try ('\n')*
        while ((tok = lexer_peek(lexer))->type == TOKEN_NEWLINE)
            lexer_pop(lexer); // token \n
    }

    return PARSER_OK;
}

enum parser_status parse_pipeline(struct ast **ast, struct lexer *lexer)
{
    struct lexer_token *tok = lexer_peek(lexer);
    // Try not
    bool not = false;
    if (tok->type == TOKEN_NOT)
    {
        not = true;
        *ast = ast_new(AST_NOT);
        lexer_pop(lexer);
    }

    // Try command
    struct ast *last_command = NULL;
    enum parser_status status_command = parse_command(&last_command, lexer);
    if (status_command == PARSER_ERROR)
    {
        ast_free(*ast);
        return handle_parser_error(status_command, &last_command);
    }

    // Try ('|' ('\n')* command)*
    struct ast *cur_pipe = NULL;
    bool first = true;
    while (true)
    {
        // Try |
        tok = lexer_peek(lexer);
        if (tok->type != TOKEN_PIPE)
            break;
        lexer_pop(lexer); // token |

        // Try ('\n')*
        while ((tok = lexer_peek(lexer))->type == TOKEN_NEWLINE)
            lexer_pop(lexer); // token \n

        // Try command
        struct ast *new_command;
        status_command = parse_command(&new_command, lexer);
        if (status_command == PARSER_ERROR)
        {
            ast_free(new_command);
            if (first)
                ast_free(last_command);
            return handle_parser_error(status_command, ast);
        }

        struct ast *ast_pipe = ast_new(AST_PIPE);
        //* Create new pipe and add command found before while loop in left
        // child
        if (first)
        {
            first = false;
            ast_pipe->left_child = last_command;
            if (not )
            {
                not = false;
                (*ast)->left_child = ast_pipe;
            }
            else
                *ast = ast_pipe;
        }
        else
        {
            cur_pipe->right_child = ast_pipe;
            ast_pipe->left_child = last_command;
        }
        cur_pipe = ast_pipe;
        last_command = new_command;
    }
    if (cur_pipe)
        cur_pipe->right_child = last_command;

    if (ast == NULL)
        ast = &last_command;
    else if ((first && !not ) || *ast == NULL)
    {
        *ast = last_command;
    }
    else if (not &&first)
        (*ast)->left_child = last_command;

    return PARSER_OK;
}

enum parser_status parse_and_or(struct ast **ast, struct lexer *lexer)
{
    // Try pipeline
    enum parser_status status_pipeline = parse_pipeline(ast, lexer);
    if (status_pipeline == PARSER_ERROR)
        return handle_parser_error(status_pipeline, ast);

    // Try (('&&'|'||') ('\n')* pipeline)*
    struct lexer_token *tok = lexer_peek(lexer);
    bool first = true;
    struct ast *last_op = NULL;
    while (true)
    {
        // Try ('&&'| '||')
        tok = lexer_peek(lexer);
        bool is_and = true;
        if (tok->type == TOKEN_AND)
            is_and = true;
        else if (tok->type == TOKEN_OR)
            is_and = false;
        else
            break;
        lexer_pop(lexer);

        // Try ('\n')*
        while ((tok = lexer_peek(lexer))->type == TOKEN_NEWLINE)
            lexer_pop(lexer); // token '\n'

        // Try pipeline
        struct ast *new_command = NULL;
        status_pipeline = parse_pipeline(&new_command, lexer);
        if (status_pipeline == PARSER_ERROR)
        {
            ast_free(new_command);
            return handle_parser_error(status_pipeline, ast);
        }

        if (first)
        {
            first = false;
            struct ast *tmp = *ast;
            *ast = ast_new(is_and ? AST_AND : AST_OR);
            last_op = *ast;
            last_op->left_child = tmp;
            last_op->right_child = new_command;
        }
        else
        {
            struct ast *tmp_left_cmd = last_op->right_child;
            last_op->right_child = ast_new(is_and ? AST_AND : AST_OR);
            last_op = last_op->right_child;
            last_op->left_child = tmp_left_cmd;
            last_op->right_child = new_command;
        }
    }
    return PARSER_OK;
}
