#ifndef AST_H
#define AST_H

enum ast_type
{
    AST_COMMAND = 0,
    AST_LIST, // < ('\n')* and_or ((';'|'\n') ('\n')* and_or)* [(';'|'\n')
              // ('\n')*]
    AST_IF,
    AST_FOR,
    AST_WHILE,
    AST_UNTIL,
    AST_CASE,
    AST_PIPE,
    AST_OR,
    AST_AND,
    AST_NOT,
    AST_REDIR,
    AST_FUNC,
    AST_EOF,
    AST_ASSIGNMENT,
    AST_CMD_SUBSTITUTION,
    AST_SUBSHELL,
    AST_CASE_SWITCH
};

enum quotes
{
    Q_NONE = 0,
    Q_DOUBLE,
    Q_SINGLE,
    Q_BACKTICK
};

struct ast
{
    enum ast_type type;
    char **value;
    char *var_name;
    enum quotes *enclosure;
    struct ast *left_child;
    struct ast *right_child;
    struct ast *condition;
};

/**
 ** \brief Allocate a new ast with the given type
 */
struct ast *ast_new(enum ast_type type);

/**
 ** \brief Recursively free the given ast
 */
void ast_free(struct ast *ast);

#endif /* !AST_H */
