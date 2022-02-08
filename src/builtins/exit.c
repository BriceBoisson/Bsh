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

int my_exit(char **args)
{
    if (args[1] == NULL)
        shell->return_code = 0;
    else
    {
        if (is_int(args[1]))
        {
            int return_code = atoi(args[1]);
            shell->return_code = return_code % 256;
        }
        else
        {
            fprintf(stderr, "bsh: exit: Illegal number: %s\n", args[1]);
            return 2;
        }
    }

    shell->exit = true;

    return 0;
}
