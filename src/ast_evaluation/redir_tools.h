#ifndef REDIR_TOOLS_H
#define REDIR_TOOLS_H

// #define _GNU_SOURCE
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

char *get_filename_from_redir(char **redir);

int get_fd_from_redir(char **redir, bool out_redir);

bool is_out_redir(char **redir);

bool is_in_redir(char **redir);

#endif // !REDIR_TOOLS_H
