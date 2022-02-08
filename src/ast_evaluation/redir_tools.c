#include "redir_tools.h"

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

char *get_filename_from_redir(char **redir)
{
    char *filename = NULL;
    int len = 0;
    while (redir[len])
        len++;
    filename = redir[len - 1];
    return filename;
}

int get_fd_from_redir(char **redir, bool out_redir)
{
    if (is_int(redir[0]))
    {
        int res = atoi(redir[0]);
        if (res > 2)
        {
            fprintf(stderr, "bsh: bad file descriptor\n");
            return -1;
        }
        return res;
    }
    return out_redir;
}

bool is_out_redir(char **redir)
{
    if (redir[0][0] == '>')
        return redir[0][1] != '&';
    if (redir[1][0] == '>')
        return redir[1][1] != '&';
    return false;
}

bool is_in_redir(char **redir)
{
    if (redir[0][0] == '<')
        return redir[0][1] != '&';
    if (redir[1][0] == '<')
        return redir[1][1] != '&';
    return false;
}
