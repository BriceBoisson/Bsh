#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ast_evaluation_tools.h"

char *get_next_free_file(void)
{
    int i = 0;
    char *file_name = calloc(25, sizeof(char));
    sprintf(file_name, "/tmp/bsh_%d", i);
    while (access(file_name, F_OK) == 0)
    {
        i++;
        free(file_name);
        file_name = calloc(25, sizeof(char));
        sprintf(file_name, "/tmp/bsh_%d", i);
    }
    return file_name;
}

char *get_file_in_var(char *path)
{
    FILE *file = fopen(path, "r");
    if (!file)
        return NULL;
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *content = malloc(sizeof(char) * (size + 1));
    fread(content, sizeof(char), size, file);
    content[size] = '\0';
    fclose(file);
    return content;
}

void free_arg(char **var)
{
    if (!var)
        return;
    int i = 0;
    while (var[i])
        free(var[i++]);
    free(var);
}

int is_in(char **condition)
{
    while ((*condition)[0] != '\0')
    {
        if (!strcmp(*condition, "in"))
            return 1;
        condition++;
    }
    return 0;
}

int is_char_name(char c)
{
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z')
        || (c >= 'a' && c <= 'z') || c == '_';
}

int is_special(char c)
{
    return c == '#' || c == '?' || c == '*' || c == '@' || c == '$' || c == '!';
}

int expand_s(char **elt, char *s, enum quotes type)
{
    int size = strlen(s);
    char *new = calloc(size + 1, sizeof(char));
    if (!new)
        return 0;
    strcpy(new, s);
    if (type == Q_NONE)
    {
        int i = 0;
        int i_new = 0;
        while (s[i] != '\0')
        {
            if (s[i] == '$')
            {
                int begin = i;
                int bracket = s[++i] == '{';
                int offset = 1;
                if (bracket)
                {
                    offset++;
                    i++;
                }
                int start = i;
                int size_var = 0;
                if (is_special(s[i]))
                {
                    i++;
                    size_var++;
                }
                else
                {
                    while (is_char_name(s[i])/*s[i] != '\0' && s[i] != ' ' && s[i] != '\t'
                        && s[i] != '$' && s[i] != '\\'
                        && (!bracket || (bracket && s[i] != '}'))*/)
                    {
                        i++;
                        size_var++;
                    }
                }
                if (bracket)
                    i++;
                if (i - begin != 1)
                {
                    char *name = calloc(size_var + 1, sizeof(char));
                    strncpy(name, s + start, size_var);
                    name[size_var] = '\0';
                    char *var = find_elt_list(shell, name);
                    if (!var)
                        var = "";
                    int new_size = strlen(var);
                    size += begin - i + new_size;
                    char *tmp = realloc(new, (size + 1) * sizeof(char));
                    if (!tmp)
                        return 0;
                    new = tmp;
                    strcpy(new + i_new, var);
                    i_new += new_size;
                    free(name);
                    if (s[i] == '\0')
                        break;
                    if (s[i] == '$')
                        continue;
                }
                else
                {
                    i--;
                    new[i_new++] = s[i++];
                }
            }
            else if (s[i] == '\\')
            {
                i++;
                new[i_new++] = s[i++];
            }
            else
                new[i_new++] = s[i++];
        }
        new[i_new] = '\0';
    }
    else if (type == Q_DOUBLE)
    {
        int i = 0;
        int i_new = 0;
        while (s[i] != '\0')
        {
            if (s[i] == '$')
            {
                int begin = i;
                int bracket = s[++i] == '{';
                int offset = 1;
                if (bracket)
                {
                    offset++;
                    i++;
                }
                int start = i;
                int size_var = 0;
                if (is_special(s[i]))
                {
                    i++;
                    size_var++;
                }
                else
                {
                    while (is_char_name(s[i])/*s[i] != '\0' && s[i] != ' ' && s[i] != '\t'
                        && s[i] != '$' && s[i] != '\\'
                        && (!bracket || (bracket && s[i] != '}'))*/)
                    {
                        i++;
                        size_var++;
                    }
                }
                if (bracket)
                    i++;
                if (i - begin != 1)
                {
                    char *name = calloc(size_var + 1, sizeof(char));
                    strncpy(name, s + start, size_var);
                    name[size_var] = '\0';
                    char *var = find_elt_list(shell, name);
                    if (!var)
                        var = "";
                    int new_size = strlen(var);
                    size += begin - i + new_size;
                    char *tmp = realloc(new, (size + 1) * sizeof(char));
                    if (!tmp)
                        return 0;
                    new = tmp;
                    strcpy(new + i_new, var);
                    i_new += new_size;
                    free(name);
                    if (s[i] == '\0')
                        break;
                    if (s[i] == '$')
                        continue;
                }
                else
                {
                    i--;
                    new[i_new++] = s[i++];
                }
            }
            else if (s[i] == '\\')
            {
                i++;
                if (s[i] == 'n')
                {
                    i++;
                    new[i_new++] = '\n';
                }
                else if (s[i] == '\"')
                {
                    i++;
                    new[i_new++] = '\"';
                }
                else if (s[i] == '\'')
                {
                    i++;
                    new[i_new++] = '\'';
                }
                new[i_new++] = s[i];
                if (s[i++] == '\0')
                    break;
            }
            else
                new[i_new++] = s[i++];
        }
        new[i_new] = '\0';
    }
    *elt = new;
    return 1;
}

int array_len(char **arr)
{
    int i = 0;
    while (arr[i] != NULL)
        i++;
    return i;
}

char **expand(char **arg, enum quotes *enclosure)
{
    char **new = calloc(array_len(arg) + 1, sizeof(char *));
    if (!new)
        return NULL;
    int ret_val = 1;
    int i = 0;
    while (arg[i] != NULL && ret_val)
    {
        ret_val = expand_s(new + i, arg[i], enclosure[i]);
        i++;
    }
    new[i] = NULL;
    if (!ret_val)
        return NULL;
    return new;
}

int str_in(char *s, char c)
{
    int i = 0;
    while (s[i] != '\0')
    {
        if (s[i++] == c)
            return 1;
    }
    return 0;
}

char *merge_arg(char **arg)
{
    if (!arg)
        return NULL;
    char *s = calloc(1, sizeof(char));
    s[0] = '\0';
    int size = 0;
    int i = 0;
    while (arg[i])
    {
        size += strlen(arg[i]);
        char *tmp = realloc(s, (size + 1) * sizeof(char));
        if (!tmp)
        {
            free(s);
            return NULL;
        }
        s = tmp;
        strcat(s, arg[i]);
        i++;
    }
    return s;
}

char **split_arg(char **arg, enum quotes *enclosure)
{
    if (!arg)
        return NULL;
    int size = array_len(arg) + 1;
    char **new = calloc(size, sizeof(char *));
    if (!new)
        return NULL;
    int ret_val = 1;
    int i = 0;
    int i_new = 0;
    while (arg[i] != NULL && ret_val)
    {
        ret_val = expand_s(new + i_new, arg[i], enclosure[i]);
        if (enclosure[i] == Q_NONE)
        {
            char *s = *(new + i_new);
            int j = 0;
            int start = 0;
            while (s[j] != '\0')
            {
                start = 0;
                if (str_in(shell->ifs, s[j]))
                {
                    start = 1;
                    size++;
                    char **tmp = realloc(new, size * sizeof(char *));
                    if (!tmp)
                        return NULL;
                    new = tmp;
                    new[i_new + 1] =
                        calloc(strlen(new[i_new] + j + 1) + 1, sizeof(char));
                    strcpy(new[i_new + 1], new[i_new] + j + 1);
                    new[i_new][j] = '\0';
                    s = new[++i_new];
                    j = 0;
                }
                else
                    j++;
            }
            if (start == 0)
                i_new++;
            i++;
        }
        else
        {
            i++;
            i_new++;
        }
    }
    new[i_new] = NULL;
    if (!ret_val)
        return NULL;
    return new;
}

int atoi_begining(char *s)
{
    int i = 0;
    int nb = 0;
    while (s[i] != '\0')
    {
        if (s[i] < '0' || s[i] > '9')
            break;
        nb = nb * 10 + (s[i++] - '0');
    }
    if (nb == 0 && i == 0)
        return -1;
    return nb;
}
