#include "builtins.h"

static void afterBackslash(char *toCheck, int *index, int fd_write)
{
    *index += 1;

    if (toCheck[*index] != '\0')
    {
        switch (toCheck[*index])
        {
        case '\\':
            write(fd_write, "\\", 1);
            break;
        case 'n':
            write(fd_write, "\n", 1);
            break;
        case 't':
            write(fd_write, "\t", 1);
            break;
        default:
            write(fd_write, "\\", 1);
            write(fd_write, &(toCheck[*index]), 1);
            break;
        }
    }
    else
        write(fd_write, "\\", 1);
}

void echo(char **args, int fd_write)
{
    bool n_option = false;
    bool e_option = false;
    int start_print = 1;

    for (; args[start_print] != NULL; start_print++)
    {
        if (!strcmp(args[start_print], "-n"))
            n_option = true;
        else if (!strcmp(args[start_print], "-e"))
            e_option = true;
        else if (!strcmp(args[start_print], "-ne")
                 || !strcmp(args[start_print], "-en"))
        {
            e_option = true;
            n_option = true;
        }
        else
            break;
    }

    if (e_option)
    {
        for (; args[start_print] != NULL; start_print++)
        {
            for (int i = 0; args[start_print][i] != '\0'; i++)
            {
                if (args[start_print][i] == '\\')
                    afterBackslash(args[start_print], &i, fd_write);
                else
                    write(fd_write, &(args[start_print][i]), 1);
            }

            if (args[start_print + 1] != NULL)
                write(fd_write, " ", 1);
        }
    }
    else
    {
        for (; args[start_print] != NULL; start_print++)
        {
            for (int i = 0; args[start_print][i] != '\0'; i++)
                write(fd_write, &(args[start_print][i]), 1);

            if (args[start_print + 1] != NULL)
                write(fd_write, " ", 1);
        }
    }

    if (!n_option)
        printf("\n");

    fflush(stdout);
}
