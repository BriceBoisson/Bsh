#include "ast.h"

#include <stdlib.h>

struct ast *ast_new(enum ast_type type)
{
    struct ast *new = calloc(1, sizeof(struct ast));
    new->type = type;
    return new;
}

void ast_free(struct ast *ast)
{
    if (ast == NULL)
        return;

    if (ast->type == AST_COMMAND || ast->type == AST_FOR
        || ast->type == AST_ASSIGNMENT || ast->type == AST_CASE
        || ast->type == AST_CASE_SWITCH)
    {
        free(ast->value);
        free(ast->enclosure);
    }
    else if (ast->type == AST_REDIR)
    {
        int i = 0;
        while (ast->value[i])
            free(ast->value[i++]);
        free(ast->value);
        free(ast->enclosure);
    }

    ast_free(ast->left_child);
    ast->left_child = NULL;

    ast_free(ast->right_child);
    ast->right_child = NULL;

    ast_free(ast->condition);
    ast->condition = NULL;

    free(ast);
}
