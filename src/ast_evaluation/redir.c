#include "redir.h"

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

static int get_open_flags(char **redir)
{
    char *type = redir[0][0] == '>' ? redir[0] : redir[1];
    switch (type[1])
    {
    case 0:
    case '|':
        return O_CREAT | O_WRONLY | O_TRUNC;
    case '>':
        return O_CREAT | O_WRONLY | O_APPEND;
    default:
        return O_CREAT | O_WRONLY | O_TRUNC;
    }
}

static bool readable(int fd)
{
    if (fd >= 0 && fd <= 2)
        return true;
    int o_accmode = 0;
    int rc = fcntl(fd, F_GETFL, &o_accmode);
    if (rc == -1)
        return false;
    rc = (o_accmode & O_ACCMODE);
    printf("%d\n", rc);
    return (rc == O_RDONLY || rc == O_RDWR);
}

static bool writeable(int fd)
{
    if (fd == 1 || fd == 2)
        return true;
    int o_accmode = 0;
    int rc = fcntl(fd, F_GETFL, &o_accmode);
    if (rc == -1)
        return false;
    rc = (o_accmode & O_ACCMODE);
    return (rc == O_WRONLY || rc == O_RDWR);
}

static void setup_out_redir(char **redir)
{
    char *filename = get_filename_from_redir(redir);
    int fd = open(filename, get_open_flags(redir), 0644);
    int ionumber = get_fd_from_redir(redir, true);
    if (ionumber < 0)
    {
        close(fd);
        return;
    }
    dup2(fd, ionumber);
    close(fd);
}

static void setup_in_redir(char **redir)
{
    char *filename = get_filename_from_redir(redir);
    int fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        fprintf(stderr, "bsh: %s: No such file or directory\n", filename);
        return;
    }
    int ionumber = get_fd_from_redir(redir, false);
    if (ionumber < 0)
    {
        close(fd);
        return;
    }
    dup2(fd, ionumber);
    close(fd);
}

static int is_out_dup(char **redir)
{
    if (redir[0][1] == '&')
        return redir[0][0] == '>';
    return redir[1][0] == '>';
}

static void setup_dup_redir(char **redir)
{
    int is_out = is_out_dup(redir);
    int fd1 = get_fd_from_redir(redir, is_out);
    char *filename = get_filename_from_redir(redir);
    if (is_out)
    {
        if (!strcmp(filename, "-"))
            close(fd1);
        else if (is_int(filename))
        {
            int fd2 = atoi(filename);
            if (fd2 == fd1)
                return;
            if (!writeable(fd2))
            {
                fprintf(stderr, "bsh: file descriptor %d is not writable\n",
                        fd2);
                return;
            }
            int tmpout = dup(fd1);
            dup2(fd2, tmpout);
            close(tmpout);
        }
    }
    else
    {
        if (!strcmp(filename, "-"))
            close(fd1);
        else if (is_int(filename))
        {
            int fd2 = atoi(filename);
            if (fd2 == fd1)
                return;
            if (!readable(fd2))
            {
                fprintf(stderr, "bsh: file descriptor %d is not readable\n",
                        fd2);
                return;
            }
            int tmpin = dup(fd1);
            dup2(fd2, tmpin);
            close(tmpin);
        }
    }
}

void exec_redirections(char ***redirs)
{
    int redirs_pos = 0;
    while (redirs[redirs_pos])
    {
        if (is_in_redir(redirs[redirs_pos]))
            setup_in_redir(redirs[redirs_pos]);
        if (is_out_redir(redirs[redirs_pos]))
            setup_out_redir(redirs[redirs_pos]);
        else
            setup_dup_redir(redirs[redirs_pos]);
        redirs_pos++;
    }
}

// int main(void)
// {
//     char **cmd = calloc(3, sizeof(char *));
//     cmd[0] = "xargs";
//     cmd[1] = "-0";
//     char ***redirs = calloc(3, sizeof(char *));
//     redirs[0] = calloc(4, sizeof(char *));
//     redirs[0][0] = "3";
//     redirs[0][1] = ">";
//     redirs[0][2] = "test.txt";
//     exec_redirections(cmd, redirs);
//     free(redirs[0]);
//     free(redirs[1]);
//     free(redirs);
//     free(cmd);
// }
