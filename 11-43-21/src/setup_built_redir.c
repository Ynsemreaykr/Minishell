#include "../include/minishell.h"
#include <fcntl.h>

// < file  (builtin için stdin redirect)
int  redir_in_builtin(t_redir *r, int *old_stdin, int *input_redirected)
{
    int infd;

    if (*input_redirected)
        return (0);
    *old_stdin = dup(0);
    if (access(r->filename, F_OK) != 0)
        return (ft_putstr_fd(r->filename, 2),
                ft_putstr_fd(": No such file or directory\n", 2), 1);
    infd = open(r->filename, O_RDONLY);
    if (infd < 0)
        return (1);
    dup2(infd, 0);
    close(infd);
    *input_redirected = 1;
    return (0);
}

// > file  (builtin için stdout overwrite)
int  redir_out_builtin(t_redir *r, int *old_stdout, int *output_redirected, int *outfd)
{
    *outfd = open(r->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (!*output_redirected)
    {
        *old_stdout = dup(1);
        *output_redirected = 1;
    }
    if (*outfd < 0)
        return (1);
    dup2(*outfd, 1);
    close(*outfd);
    return (0);
}

// >> file (builtin için stdout append)
int  redir_append_builtin(t_redir *r, int *old_stdout, int *output_redirected, int *outfd)
{
    *outfd = open(r->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (!*output_redirected)
    {
        *old_stdout = dup(1);
        *output_redirected = 1;
    }
    if (*outfd < 0)
        return (1);
    dup2(*outfd, 1);
    close(*outfd);
    return (0);
}

void	redirect_heredoc_for_builtin(t_cmd *cmd, int *old_stdin,
					int *input_redirected)
{
	t_heredoc	*h;
	int			hpipe[2];

	if (!cmd->heredocs)
		return ;
	*old_stdin = dup(0);
	h = cmd->heredocs;
	while (h->next)
		h = h->next;
	if (!h->content)
		return ;
	if (pipe(hpipe) != 0)
		return ;
	write(hpipe[1], h->content, ft_strlen(h->content));
	close(hpipe[1]);
	dup2(hpipe[0], 0);
	close(hpipe[0]);
	*input_redirected = 1;
}