#include <err.h>
#include <errno.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "ast_evaluation_tools.h"
#include "builtins.h"

int exec_with_fork(char **cmd, int i, int pipe_nb, int ***fds)
{
    int pid = fork();
    if (pid == -1)
    {
        fprintf(stderr, "bsh: fork error\n");
    }
    else if (pid == 0)
    {
        if (i == 0)
        {
            dup2((*fds)[i][1], 1);
            close((*fds)[i][0]);
            close((*fds)[i][1]);
            if (is_builtin(cmd[0]))
            {
                find_command(cmd, 1);
                kill(getpid(), SIGKILL);
                return shell->return_code;
            }
            else
                execvp(cmd[0], cmd);
            fprintf(stderr, "bsh: command not found: %s\n", cmd[0]);
            kill(getpid(), SIGKILL);
            return 127;
        }
        else if (i == pipe_nb)
        {
            dup2((*fds)[i - 1][0], 0);
            close((*fds)[i - 1][0]);
            close((*fds)[i - 1][1]);
            if (is_builtin(cmd[0]))
            {
                find_command(cmd, 1);
                kill(getpid(), SIGKILL);
                return shell->return_code;
            }
            else
                execvp(cmd[0], cmd);
            fprintf(stderr, "bsh: command not found: %s\n", cmd[0]);
            kill(getpid(), SIGKILL);
            return 127;
        }
        else
        {
            dup2((*fds)[i - 1][0], 0);
            dup2((*fds)[i][1], 1);
            close((*fds)[i - 1][0]);
            close((*fds)[i - 1][1]);
            close((*fds)[i][0]);
            close((*fds)[i][1]);
            if (is_builtin(cmd[0]))
            {
                find_command(cmd, 1);
                kill(getpid(), SIGKILL);
                return shell->return_code;
            }
            else
                execvp(cmd[0], cmd);
            fprintf(stderr, "bsh: command not found: %s\n", cmd[0]);
            kill(getpid(), SIGKILL);
            return 127;
        }
    }
    else
    {
        if (i != 0)
        {
            close((*fds)[i - 1][0]);
            close((*fds)[i - 1][1]);

            int wstatus;
            if (waitpid(pid, &wstatus, 0) == -1)
                shell->return_code = 1;

            if (!WIFEXITED(wstatus))
                shell->return_code = 127;
            else
                shell->return_code = WEXITSTATUS(wstatus);
        }
        return shell->return_code;
    }
    return shell->return_code;
}

int exec_pipe(char ***args, enum quotes **enclosure, int pipe_nb)
{
    int res = 0;
    int **fds = calloc(pipe_nb, sizeof(int *));

    for (int i = 0; i < pipe_nb; i++)
        fds[i] = calloc(2, sizeof(int));

    for (int i = 0; i <= pipe_nb; i++)
    {
        if (i != pipe_nb)
        {
            if (pipe(fds[i]) == -1)
            {
                fprintf(stderr, "bsh: bad pipe\n");
            }
        }
        char **val = expand(args[i], enclosure[i]);
        // if (is_builtin(val[0]))
        //     res = exec_without_fork(val, i, pipe_nb, &fds);
        // else
        res = exec_with_fork(val, i, pipe_nb, &fds);
        char *tmp = val[0];
        int pos = 0;
        while (tmp)
        {
            free(tmp);
            tmp = val[++pos];
        }
        free(val);
    }
    for (int i = 0; i < pipe_nb; i++)
        free(fds[i]);
    free(fds);
    free(args);
    free(enclosure);
    return res;
}
