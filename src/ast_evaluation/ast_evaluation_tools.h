#ifndef AST_EVALUATION_TOOLS_H
#define AST_EVALUATION_TOOLS_H

#include "bsh.h"
#include "ast.h"
#include "var_list.h"

// include builtin
extern struct shell *shell;
// int call_builtin(char *cmd);
int is_builtin(char *);
int call_exec(char **cmd);
int is_in(char **condition);
char **expand(char **arg, enum quotes *enclosure);
char **split_arg(char **arg, enum quotes *enclosure);
char *merge_arg(char **arg);
int atoi_begining(char *s);
int exec_pipe(char ***args, enum quotes **enclosure, int pipe_nb);
void free_arg(char **var);
char *get_next_free_file(void);
char *get_file_in_var(char *path);

#endif /* !AST_EVALUATION_TOOLS_H */
