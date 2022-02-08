#include "builtins.h"

int find_command(char **args, int fd_write)
{
    if (!strcmp(args[0], "echo"))
    {
        echo(args, fd_write);
        return 0;
    }
    if (!strcmp(args[0], "cd"))
        return cd(args);
    if (!strcmp(args[0], "continue"))
        return my_continue(args);
    if (!strcmp(args[0], "break"))
        return my_break(args);
    if (!strcmp(args[0], "exit"))
        return my_exit(args);
    if (!strcmp(args[0], "export"))
        return export(args);
    if (!strcmp(args[0], "unset"))
        return unset(args);
    if (!strcmp(args[0], "."))
        return dot(args);
    else
        return 1;
}
