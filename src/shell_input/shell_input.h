#ifndef SHELL_INPUT_H
#define SHELL_INPUT_H

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include "../bsh.h"

extern struct shell *shell;

/**
 * @brief Get the input for the shell and send it to the lexer
 *
 * @param argc number of arguments
 * @param argv list of arguments
 * @return int
 */
int get_input(int argc, char **argv);

#endif // !SHELL_INPUT_H
