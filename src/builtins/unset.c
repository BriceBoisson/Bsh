#include "../functions/functions.h"
#include "../variables/var_list.h"
#include "builtins.h"

int unset(char **args)
{
    bool v_option = false;
    bool f_option = false;
    int start_print = 1;

    for (; args[start_print] != NULL; start_print++)
    {
        if (!strcmp(args[start_print], "-v"))
            v_option = true;
        else if (!strcmp(args[start_print], "-f"))
            f_option = true;
        else if (!strcmp(args[start_print], "-fv")
                 || !strcmp(args[start_print], "-vf"))
        {
            v_option = true;
            f_option = true;
        }
        else
        {
            if (args[start_print][0] == '-')
            {
                fprintf(stderr, "bsh: unset: Illegal option %s\n",
                        args[start_print]);
                return 1;
            }

            break;
        }
    }

    if (!v_option && !f_option)
        v_option = true;

    for (int i = 0; args[start_print][i] != '\0'; i++)
    {
        if ((args[start_print][i] >= 'a' && args[start_print][i] <= 'z')
            || (args[start_print][i] >= 'A' && args[start_print][i] <= 'Z')
            || (args[start_print][i] >= '0' && args[start_print][i] <= '9')
            || args[start_print][i] == '_')
            continue;

        fprintf(stderr, "bsh: unset: %s: bad variable name\n",
                args[start_print]);
        return 1;
    }

    if (v_option)
    {
        del_name(shell, args[start_print]);
        unsetenv(args[start_print]);
    }

    if (f_option)
        del_fun_name(shell, args[start_print]);

    return 0;
}
