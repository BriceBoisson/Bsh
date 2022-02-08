#include "builtins.h"

static bool is_int(char *str)
{
    int i = 0;
    while (str[i])
    {
        if (str[i] < '0' || str[i] > '9')
            return false;
        i++;
    }
    return true;
}

int my_break(char **args)
{
    if (args[1] == NULL)
    {
        shell->brk = 1;
        return 1;
    }

    if (is_int(args[1]))
    {
        shell->brk = atoi(args[1]);
        return shell->brk;
    }

    fprintf(stderr, "bsh: break: Illegal number: %s\n", args[1]);
    return -1;
}
