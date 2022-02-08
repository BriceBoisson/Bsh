#include "shell_input.h"

#include "parser.h"
#include "var_list.h"

static int shell_prompt(void)
{
    char *input = NULL;
    size_t input_len = 0;
    while (!shell->exit)
    {
        if (shell->return_code)
            fprintf(stderr,
                    "\033[1m\033[31m➜  \033[1m\033[36mbsh \033[1m\033[33m✗ "
                    "\033[0;37m");
        else
            fprintf(stderr,
                    "\033[1m\033[32m➜  \033[1m\033[36mbsh \033[1m\033[33m✗ "
                    "\033[0;37m");
        fflush(stderr);
        int line = 0;
        if (getline(&input, &input_len, stdin) < 1)
        {
            free(input);
            input = NULL;
            input_len = 0;
            break;
        }
        if (!input)
        {
            if (!line)
                fprintf(stderr, "\n");
            continue;
        }
        if (!strcmp(input, "exit\n"))
        {
            shell->exit = 1;
            free(input);
            continue;
        }
        parse_input(input, NULL);
        free(input);
        input = NULL;
        input_len = 0;
    }
    return shell->return_code;
}

static char *get_file_content(char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        fprintf(stderr, "bsh: Can't open %s\n", filename);
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *content = calloc(size + 1, sizeof(char));
    fread(content, 1, size, file);
    content[size] = 0;
    fclose(file);
    return content;
}

static char **copy_args(char **argv)
{
    int i = 0;
    char **args = NULL;
    while (argv[i])
    {
        args = realloc(args, (i + 2) * sizeof(char *));
        args[i] = strdup(argv[i]);
        i++;
    }
    shell->nb_args = i - 1;
    if (args)
        args[i] = NULL;
    else
        args = calloc(1, sizeof(char *));
    return args;
}

int get_input(int argc, char **argv)
{
    char *input = NULL;
    size_t input_len = 0;
    if (argc < 2)
    {
        shell->args = calloc(2, sizeof(char *));
        shell->args[0] = strdup(argv[0]);
        new_var(shell, shell->args);
        int c;
        if (!isatty(STDIN_FILENO))
        {
            while (read(STDIN_FILENO, &c, 1) > 0)
            {
                if (c == EOF)
                    break;
                input = realloc(input, (input_len + 2) * sizeof(char));
                input[input_len++] = c;
            }
        }
        else
            return shell_prompt();
    }
    else
    {
        int i = 1;
        if (!strcmp(argv[i], "-c"))
        {
            shell->args = calloc(2, sizeof(char *));
            shell->args[0] = strdup(argv[0]);
            new_var(shell, shell->args);
            input = strdup(argv[++i]);
        }
        else
        {
            input = get_file_content(argv[i]);
            shell->args = copy_args(argv + 1);
            new_var(shell, shell->args);
        }
        if (!input)
            return 1;
        input_len = strlen(input);
    }
    if (input)
    {
        input[input_len] = '\0';
        parse_input(input, NULL);
        free(input);
    }
    return shell->return_code;
}
