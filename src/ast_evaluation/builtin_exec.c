#include <string.h>

#include "ast_evaluation_tools.h"
#include "builtins.h"

/*
 * The main objectif of this source code is to call the builtin implemented
 * inside of the bsh project
 */

int is_builtin(char *cmd)
{
    if (!strcmp(cmd, "echo"))
        return 1;
    if (!strcmp(cmd, "cd"))
        return 1;
    if (!strcmp(cmd, "break"))
        return 1;
    if (!strcmp(cmd, "continue"))
        return 1;
    if (!strcmp(cmd, "exit"))
        return 1;
    if (!strcmp(cmd, "export"))
        return 1;
    if (!strcmp(cmd, "unset"))
        return 1;
    if (!strcmp(cmd, "."))
        return 1;
    return 0;
}

/*int call_echo(char *cmd)
{
    int i = 0;
    // int flag = 0;
    while (cmd[i] != '\0')
    {
        while (!(cmd[i] > ' '))
            i++;
        if (!strcmp(cmd, "-e", i))
            if (i != 1 && i != 3)
                flag++;
        if (!strcmp(cmd, "-n", i))
            if (i < 2)
                flag += 2;
        if (!strcmp(cmd, "-ne", i))
            flag = 3;
    }

    printf("%s\n", cmd);
    echo(cmd + i, 1);
    return 0;
}*/

/*int call_builtin(char **cmd)
{
    int i = 0;
    while (cmd[i] != ' ')
        i++;
    if (!strncmp(cmd, "echo", i))
    {
        return call_echo(cmd + i);
    }
    return 0;
}*/
