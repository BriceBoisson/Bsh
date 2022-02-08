#include "functions.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../bsh.h"

int push_elt_fun(struct shell *sh, char *name, struct ast *fun)
{
    struct functions *tmp = sh->functions;
    while (tmp && strcmp(tmp->name, name))
        tmp = tmp->next;
    if (tmp)
        tmp->function = fun;
    else
    {
        struct functions *new = calloc(1, sizeof(struct functions));
        if (!new)
            return 1;
        new->name = calloc(strlen(name) + 1, sizeof(char));
        if (!new->name)
        {
            free(new);
            return 1;
        }
        strcpy(new->name, name);
        new->function = fun;
        new->next = sh->functions;
        sh->functions = new;
    }
    return 0;
}

struct ast *find_elt_fun(struct shell *sh, char *name)
{
    struct functions *tmp = sh->functions;
    while (tmp && strcmp(tmp->name, name))
        tmp = tmp->next;
    // printf("%s\n", tmp->value);
    if (tmp)
        return tmp->function;
    return NULL;
}

void free_fun_sub(struct shell *sh)
{
    struct functions *fun = sh->functions;
    while (fun)
    {
        struct functions *tmp = fun;
        fun = fun->next;
        free(tmp->name);
        free(tmp);
    }
}

int del_fun_name(struct shell *sh, char *name)
{
    struct functions *actual = sh->functions;
    struct functions *previous = sh->functions;
    int index = 0;

    while (actual)
    {
        if (!strcmp(actual->name, name))
        {
            if (index == 0)
                sh->functions = actual->next;
            else
                previous->next = actual->next;

            free(actual->name);
            free(actual);

            return 1;
        }

        index++;
        previous = actual;
        actual = actual->next;
    }

    return 0;
}

struct functions *fun_list_cpy(struct shell *sh)
{
    struct functions *new = NULL;
    struct functions *fun = sh->functions;
    while (fun)
    {
        struct functions *tmp = new;
        new = calloc(1, sizeof(struct functions));
        if (!new)
            return NULL;
        new->name = calloc(strlen(fun->name) + 1, sizeof(char));
        if (!new->name)
        {
            free(new);
            return NULL;
        }
        strcpy(new->name, fun->name);
        new->function = fun->function;
        new->next = tmp;
        fun = fun->next;
    }
    return new;
}