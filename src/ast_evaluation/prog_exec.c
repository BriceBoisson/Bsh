#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

#include "ast_evaluation_tools.h"

/*
 * The main objectif of this source code is to call some programe of the
 * computer using fork and exec
 */

int call_exec(char **cmd)
{
    pid_t cpid = fork();
    if (cpid == -1)
    {
        perror("bsh");
        return 1;
    }
    else if (!cpid)
    {
        execvp(cmd[0], cmd);
        fprintf(stderr, "bsh: command not found: %s\n", cmd[0]);
        kill(getpid(), SIGKILL);
        return 127;
    }
    else
    {
        int cstatus = 0;
        if (waitpid(cpid, &cstatus, 0) == -1)
        {
            return 1;
        }

        if (!WIFEXITED(cstatus))
        {
            return 127;
        }

        return WEXITSTATUS(cstatus);
    }
}
