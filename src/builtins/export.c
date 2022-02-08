#include "../variables/var_list.h"
#include "builtins.h"

int export(char **args)
{
    if (args[1] == NULL)
    {
        fprintf(stderr,
                "bsh: export: expected export <name> or export <name>=<value> "
                "but got export\n");
        return 0;
    }

    for (int i = 0; args[1][i] != '\0'; i++)
    {
        if ((args[1][i] >= 'a' && args[1][i] <= 'z')
            || (args[1][i] >= 'A' && args[1][i] <= 'Z')
            || (args[1][i] >= '0' && args[1][i] <= '9') || args[1][i] == '_')
            continue;

        fprintf(stderr, "bsh: export %s: bad variable name\n", args[1]);
        return 2;
    }

    if (args[2] == NULL)
    {
        char *value = find_elt_list(shell, args[1]);
        if (value != NULL)
        {
            setenv(args[1], value, 1);
        }
    }
    else
    {
        setenv(args[1], args[2], 1);
        push_elt_list(shell, args[1], args[2]);
    }

    return 0;
}
