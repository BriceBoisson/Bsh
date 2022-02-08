#include <stdio.h>

#include "ast.h"
#include "ast_evaluation_tools.h"

void pretty_print(struct ast *ast)
{
    if (ast)
    {
        if (ast->type == AST_IF || ast->type == AST_FOR
            || ast->type == AST_WHILE || ast->type == AST_UNTIL)
        {
            if (ast->type == AST_IF)
            {
                printf("if { ");
                pretty_print(ast->condition);
                printf("}; then { ");
                pretty_print(ast->left_child);
                printf("}");
                if (ast->right_child)
                {
                    printf(" else { ");
                    pretty_print(ast->right_child);
                    printf("}");
                }
            }
            else if (ast->type == AST_FOR)
            {
                printf("for { ");
                for (int i = 0; ast->value[i]; i++)
                {
                    printf("%s ", ast->value[i]);
                }
                printf("}; do { ");
                pretty_print(ast->left_child);
                printf("}");
            }
            else if (ast->type == AST_WHILE)
            {
                printf("while { ");
                pretty_print(ast->condition);
                printf("}; do { ");
                pretty_print(ast->left_child);
                printf("}");
            }
            else if (ast->type == AST_UNTIL)
            {
                printf("until { ");
                pretty_print(ast->condition);
                printf("}; do { ");
                pretty_print(ast->left_child);
                printf("}");
            }
        }
        else if (ast->type == AST_AND)
        {
            printf("{ ");
            pretty_print(ast->left_child);
            printf("} && { ");
            pretty_print(ast->right_child);
            printf("}");
        }
        else if (ast->type == AST_OR)
        {
            printf("{ ");
            pretty_print(ast->left_child);
            printf("} OR { ");
            pretty_print(ast->right_child);
            printf("}");
        }
        else if (ast->type == AST_REDIR)
        {
            pretty_print(ast->left_child);
            printf(" %s ", ast->value[0]);
            pretty_print(ast->right_child);
        }
        else if (ast->type == AST_PIPE)
        {
            pretty_print(ast->left_child);
            printf(" | ");
            pretty_print(ast->right_child);
        }
        else if (ast->type == AST_COMMAND)
        {
            if (ast->value[0])
            {
                printf("%s", ast->value[0]);
                for (size_t i = 1; ast->value[i] != NULL; i++)
                    printf(" %s", ast->value[i]);
            }
        }
        else if (ast->type == AST_LIST)
        {
            pretty_print(ast->right_child);
            printf("; ");
            pretty_print(ast->left_child);
        }
        else if (ast->type == AST_NOT)
        {
            printf("! ");
            pretty_print(ast->left_child);
        }
        else if (ast->type == AST_ASSIGNMENT)
        {
            printf("%s=", ast->var_name);
            for (size_t i = 0; ast->value && ast->value[i] != NULL; i++)
                printf("%s ", ast->value[i]);
        }
        else if (ast->type == AST_FUNC)
        {
            if (ast->var_name)
                printf("%s() ", ast->var_name);
            printf("{ ");
            pretty_print(ast->left_child);
            printf("}");
        }
        else if (ast->type == AST_CMD_SUBSTITUTION)
        {
            printf("$(");
            pretty_print(ast->left_child);
            printf(")");
        }
        else if (ast->type == AST_SUBSHELL)
        {
            printf("(");
            pretty_print(ast->left_child);
            printf(")");
        }
        else if (ast->type == AST_CASE)
        {
            printf("case %s in ", ast->value[0]);
            pretty_print(ast->left_child);
            printf("esac");
        }
        else if (ast->type == AST_CASE_SWITCH)
        {
            printf("%s", ast->value[0]);
            for (int i = 1; ast->value[i]; i++)
                printf("|%s", ast->value[i]);
            printf(") ");
            pretty_print(ast->left_child);
            printf(";; ");
            pretty_print(ast->right_child);
        }
        else if (ast->type == AST_EOF)
            printf("EOF\n");
    }
}
