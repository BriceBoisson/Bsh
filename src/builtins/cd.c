#include "builtins.h"
#include "var_list.h"

int cd(char **args)
{
    int len = 0;
    while (args[len])
        len++;
    if (len < 2)
        return 0;
    if (!strcmp(args[1], "-"))
    {
        chdir(shell->oldpwd);
        char *swap = shell->oldpwd;
        shell->oldpwd = shell->pwd;
        shell->pwd = swap;
        printf("%s\n", shell->pwd);
        setenv("OLDPWD", shell->oldpwd, 1);
        setenv("PWD", shell->pwd, 1);
        fflush(stdout);
        return 0;
    }

    int error_chdir = chdir(args[1]);

    if (error_chdir == -1)
    {
        fprintf(stderr, "bsh: cd: can't cd to %s\n", args[1]);
        return 1;
    }

    shell->oldpwd = strcpy(shell->oldpwd, shell->pwd);
    push_elt_list(shell, "OLDPWD", shell->pwd);

    getcwd(shell->pwd, 2048);

    setenv("OLDPWD", shell->oldpwd, 1);
    setenv("PWD", shell->pwd, 1);

    return 0;
}
