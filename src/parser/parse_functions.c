#include "parser.h"

enum parser_status parse_funcdec(struct ast **ast, struct lexer *lexer)
{
    struct lexer_token *tok = lexer_peek(lexer);

    *ast = ast_new(AST_FUNC);

    // Try WORD
    if (tok->type != TOKEN_WORD)
        return handle_parser_error(PARSER_ERROR, ast);

    (*ast)->var_name = tok->value;
    lexer_pop(lexer); // token WORD

    // Try (
    tok = lexer_peek(lexer);
    if (tok->type != TOKEN_PARENTHESIS_OPEN)
        return handle_parser_error(PARSER_ERROR, ast);
    lexer_pop(lexer); // token (

    // Try )
    tok = lexer_peek(lexer);
    if (tok->type != TOKEN_PARENTHESIS_CLOSE)
        return handle_parser_error(PARSER_ERROR, ast);
    lexer_pop(lexer); // token )

    // Try ('\n')*
    while ((tok = lexer_peek(lexer))->type == TOKEN_NEWLINE)
        lexer_pop(lexer); // token \n

    enum parser_status status_shell_cmd = parse_shell_command(ast, lexer);
    if (status_shell_cmd == PARSER_ERROR)
        return handle_parser_error(status_shell_cmd, ast);

    return PARSER_OK;
}
