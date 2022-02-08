#ifndef BUILTINS_H
#define BUILTINS_H

#include <dirent.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "bsh.h"
#include "lexer.h"

extern struct shell *shell;

/**
 * @brief : Check which builtin command is received and execute it.
 *
 * @param toExecute : Builtin command starting from the bultin to the end
 *                      of it's argument.
 * @return int : 0 on success, 1 on failure.
 */
int find_command(char **toExecute, int fd_write);

/**
 * @brief Execute cd command.
 *
 * @param args the list of arguments
 * @return int return code
 */
int cd(char **args);

/**
 * @brief The echo command.
 *
 * @param args the list of arguments
 */
void echo(char **args, int fd_write);

/**
 * @brief  @brief Modify the shell->exit and shell->return_code parameter in the
 * global variable shell
 *
 * @param args the list of arguments
 * @return int return 0 on success, 1 on failure
 */
int my_exit(char **args);

/**
 * @brief Put a variable to the exported variable-list, if it doesn't exist, it
 * creates it.
 *
 * @param args the list of arguments.
 * @return int return 0 on success, 1 on failure.
 */
int export(char **args);

/**
 * @brief The continue command.
 *
 * @param args the list of arguments.
 * @return int the number of enclosing loops to continue. -1 on failure.
 */
int my_continue(char **args);

/**
 * @brief The break command.
 *
 * @param args the list of arguments.
 * @return int the number of enclosing loops to break. -1 on failure.
 */
int my_break(char **args);

/**
 * @brief The unset command.
 *
 * @param args the list of arguments.
 * @return int return 0 on success, -1 on failure.
 */
int unset(char **args);

/**
 * @brief The dot builtin
 *
 * @param argv the list of arguments.
 * @return int
 */
int dot(char **argv);

#endif
