#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "bsh.h"

int push_elt_fun(struct shell *sh, char *name, struct ast *fun);
struct ast *find_elt_fun(struct shell *sh, char *name);
void free_fun_sub(struct shell *sh);
int del_fun_name(struct shell *sh, char *name);
struct functions *fun_list_cpy(struct shell *sh);

#endif
