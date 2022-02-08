#include <errno.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "bsh.h"
#include "lexer.h"
#include "parser.h"
#include "shell_input.h"
#include "var_list.h"

extern struct shell *shell;

static char **split_path(char *path)
{
    char *rest = path;
    char *token;
    char **paths = NULL;
    int paths_nb = 0;
    while ((token = strtok_r(rest, ":", &rest)) != NULL)
    {
        paths = realloc(paths, sizeof(char *) * (paths_nb + 2));
        paths[paths_nb++] = strdup(token);
    }
    paths[paths_nb] = NULL;
    return paths;
}

static char *find_in_path(const char *input)
{
    char *path_tmp = getenv("PATH");
    char *path = calloc(strlen(path_tmp) + 1, sizeof(char));
    strcpy(path, path_tmp);
    if (!path)
    {
        fprintf(stderr, "bsh: .: %s: not found\n", input);
        shell->return_code = 2;
        free(path);
        return NULL;
    }
    char **path_single = split_path(path);
    if (!path_single)
    {
        fprintf(stderr, "bsh: .: %s: not found\n", input);
        shell->return_code = 2;
        free(path);
        return NULL;
    }
    int i = 0;
    bool found = false;
    char *final_path = NULL;
    while (path_single[i] && !found)
    {
        char *full_path =
            calloc(strlen(path_single[i]) + strlen(input) + 2, sizeof(char));
        strcpy(full_path, path_single[i]);
        strcat(full_path, "/");
        strcat(full_path, input);
        struct stat sb;
        if (access(full_path, F_OK) == 0
            && (stat(path, &sb) == 0 && sb.st_mode & S_IXUSR))
        {
            found = true;
            final_path = full_path;
            continue;
        }
        free(full_path);
        free(path_single[i]);
        i++;
    }
    if (!final_path)
    {
        fprintf(stderr, "bsh: .: %s: not found\n", input);
        shell->return_code = 2;
    }
    free(path);
    free(path_single);
    return final_path;
}

static char get_first_char(const char *arg)
{
    int pos = 0;
    while (arg[pos] != 0 && arg[pos] == '.')
        pos++;
    return arg[pos];
}

char *get_file_content(const char *path)
{
    if (get_first_char(path) != '/')
        path = find_in_path(path);
    if (!path)
        return NULL;
    char *buffer = 0;
    long length;
    FILE *f = fopen(path, "r");
    if (!f)
    {
        shell->return_code = 127;
        fprintf(stderr, "bsh: .: Can't open %s\n", path);
        return NULL;
    }
    struct stat sb;
    if (!(stat(path, &sb) == 0 && sb.st_mode & S_IXUSR))
    {
        shell->return_code = 126;
        fprintf(stderr, "bsh: %s: Permission denied\n", path);
        fclose(f);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    length = ftell(f);
    fseek(f, 0, SEEK_SET);
    buffer = calloc(length + 1, sizeof(char));
    if (buffer)
    {
        fread(buffer, 1, length, f);
    }
    fclose(f);
    return buffer;
}

static void restore_shell(struct var *vars, struct var_stack *stack)
{
    free_list(shell);
    shell->var_list = vars;
    shell->var_stack = stack;
}

int dot(char **argv)
{
    struct var *save_var_list = shell->var_list;
    struct var_stack *save_var_stack = shell->var_stack;
    shell->var_list = NULL;
    shell->var_stack = NULL;
    char **args = calloc(2, sizeof(char *));
    new_var(shell, args);
    if (!argv[1])
    {
        free(args);
        restore_shell(save_var_list, save_var_stack);
        return 0;
    }
    char *buf = get_file_content(argv[1]);
    if (!buf)
    {
        free(args);
        restore_shell(save_var_list, save_var_stack);
        return shell->return_code;
    }
    parse_input(buf, NULL);
    free(buf);
    free(args);
    restore_shell(save_var_list, save_var_stack);
    return shell->return_code;
}
