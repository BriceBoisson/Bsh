#include "bsh.h"

#include <stdio.h>
#include <string.h>

#include "functions.h"
#include "lexer.h"
#include "loop_stack.h"
#include "shell_input.h"
#include "var_list.h"

struct shell *shell;

static void init_shell(int argc, char **argv)
{
    shell = calloc(1, sizeof(struct shell));
    shell->pid = getppid();
    shell->pretty_print = argc > 1 ? !strcmp(argv[1], "--pretty-print") : false;
    if (shell->pretty_print)
    {
        printf("Pretty print enabled\n");
        argc--;
        argv++;
    }
    shell->verbose = argc > 1 ? !strcmp(argv[1], "--verbose") : false;
    if (shell->verbose)
        printf("Verbose mode enabled\n");
    shell->oldpwd = calloc(2048, sizeof(char));
    if (getenv("OLDPWD"))
        strcpy(shell->oldpwd, getenv("OLDPWD"));
    else if (getcwd(shell->oldpwd, 2048) == NULL)
        shell->exit = true;

    shell->pwd = calloc(2048, sizeof(char));
    if (!shell->exit && getcwd(shell->pwd, 2048) == NULL)
        shell->exit = true;

    // TODO: what are the shell parameters?
    shell->args = NULL;
    shell->nb_args = 0;

    shell->ifs = calloc(100, sizeof(char));
    strcpy(shell->ifs, " \t\n");
    shell->uid = getuid();
    shell->var_list = NULL;
    shell->var_stack = NULL;
    shell->functions = NULL;
    shell->loop_stack = NULL;
    shell->random_nb = NULL;
    // append param shell->var_stack
}

void free_shell(void)
{
    int i = 0;
    while (shell->args && shell->args[i])
        free(shell->args[i++]);
    free(shell->args);
    free_list(shell);
    free_fun_sub(shell);
    free(shell->oldpwd);
    free(shell->pwd);
    free(shell->ifs);
    if (shell->random_nb)
        free(shell->random_nb);
    struct lexer_alias *alias = shell->alias_list;
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
    free_loop(shell);
    free(shell);
}

void print_shell(void)
{
    printf("bsh\n");
    printf("  + oldpwd: %s\n", shell->oldpwd);
    printf("  + pwd: %s\n", shell->pwd);
    printf("  + args (%d):\n", shell->nb_args);
    for (int i = 0; i < shell->nb_args; i++)
        printf("    %s\n", shell->args[i]);
    printf("  + ifs:");
    for (int i = 0; shell->ifs[i] != '\0'; i++)
        printf(" %d", shell->ifs[i]);
    printf("\n");
    printf("  + uid: %d\n", shell->uid);
    printf("  + exit: %d\n", shell->exit);
    printf("  + last_return_code: %d\n", shell->return_code);
    printf("\n");
}

int main(int argc, char **argv)
{
    init_shell(argc, argv);
    if (shell->exit)
    {
        free_shell();
        fprintf(stderr, "bsh: error during initialization.\n");
        return 1;
    }
    // print_shell();
    int res;
    if (shell->pretty_print && shell->verbose)
        res = get_input(argc - 2, argv + 2);
    else if (shell->pretty_print || shell->verbose)
        res = get_input(argc - 1, argv + 1);
    else
        res = get_input(argc, argv);
    free_shell();
    return res;
    char *input = calloc(49, sizeof(char));
    strcpy(input, "if test then bsh.c\n then echo 'bsh.c exists'\n fi");
    struct lexer *lexer = lexer_create(input);
    lexer_build(lexer);
    lexer_free(lexer);
    return (0);
}
