#ifndef SHELL_H
#define SHELL_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

struct var
{
    char *name;
    char *value;
    struct var *next;
};

struct var_stack
{
    struct var *var_list;
    struct var_stack *next;
};

struct loop_stack
{
    struct ast *loop;
    struct loop_stack *next;
};

struct functions
{
    char *name;
    struct ast *function;
    struct functions *next;
};

struct shell
{
    bool pretty_print;
    char *oldpwd;
    char *pwd;
    bool exit;
    char **args;
    int nb_args;
    char *ifs;
    uid_t uid;
    int return_code;
    char *random_nb;
    bool verbose;
    bool interupt;
    struct var_stack *var_stack;
    struct var *var_list;
    struct loop_stack *loop_stack;
    struct functions *functions;
    int ctn; // continue is a keyword
    int brk; // break is a keyword
    pid_t pid;
    struct lexer_alias *alias_list;
};

/**
 * @brief Print the shell structure.
 *
 */
void print_shell(void);

/**
 * @brief Free the shell structure.
 *
 */
void free_shell(void);

#endif // !SHELL_H
