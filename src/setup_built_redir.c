/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup_built_redir.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/29 02:50:41 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/08/29 02:52:14 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include <fcntl.h>

int	redir_in_builtin(t_redir *r, int *input_redirected)
{
	int	infd;

	if (access(r->filename, F_OK) != 0)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(r->filename, 2);
		ft_putstr_fd(": No such file or directory\n", 2);
		return (1);
	}
	infd = open(r->filename, O_RDONLY);
	if (infd < 0)
		return (1);	
	dup2(infd, STDIN_FILENO);
	close(infd);
	*input_redirected = 1;
	return (0);
}

int	redir_out_builtin(t_redir *r, int *output_redirected, int *outfd)
{
	*outfd = open(r->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (!*output_redirected)
		*output_redirected = 1;
	if (*outfd < 0)
		return (1);
	dup2(*outfd, STDOUT_FILENO);
	close(*outfd);
	return (0);
}

int	redir_append_builtin(t_redir *r, int *output_redirected, int *outfd)
{
	*outfd = open(r->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (!*output_redirected)
		*output_redirected = 1;
	if (*outfd < 0)
		return (1);
	dup2(*outfd, STDOUT_FILENO);
	close(*outfd);
	return (0);
}

void	redirect_heredoc_for_builtin(t_cmd *cmd, int *input_redirected)
{
	t_heredoc	*h;
	int			hpipe[2];

	if (!cmd->heredocs)
		return ;
	h = cmd->heredocs;
	while (h->next)
		h = h->next;
	if (!h->content)
		return ;
	if (pipe(hpipe) != 0)
		return ;
	write(hpipe[1], h->content, ft_strlen(h->content));
	close(hpipe[1]);
	dup2(hpipe[0], STDIN_FILENO);
	close(hpipe[0]);	
	*input_redirected = 1;
}

static int	apply_single_redir_builtin(t_redir *r, int *input_redirected,
				int *output_redirected, int *outfd)
{
	if (r->type == REDIR_IN)
	{
		if (redir_in_builtin(r, input_redirected) == 1)
			return (-1);
	}
	else if (r->type == REDIR_OUT)
	{
		if (redir_out_builtin(r, output_redirected, outfd) == 1)
			return (-1);
	}
	else if (r->type == REDIR_APPEND)
	{
		if (redir_append_builtin(r, output_redirected, outfd) == 1)
			return (-1);
	}
	return (0);
}

int	setup_redirections_for_builtin(t_cmd *cmd)
{
	int			input_redirected;
	int			output_redirected;
	int			outfd;
	t_redir		*r;

	input_redirected = 0;
	output_redirected = 0;
	outfd = -1;
	if (cmd->heredocs)
		redirect_heredoc_for_builtin(cmd, &input_redirected);
	if (cmd->redirs)
	{
		r = cmd->redirs;
		while (r)
		{
			if (apply_single_redir_builtin(r, &input_redirected,
					&output_redirected, &outfd) == -1)
				return (-1);
			r = r->next;
		}
	}
	return (0);
}
