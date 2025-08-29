/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup_redir.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/29 02:46:35 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/08/29 02:49:59 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include <fcntl.h>

int	handle_redir_in(t_redir *redir)
{
	int	in;

	if (access(redir->filename, F_OK) != 0)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(redir->filename, 2);
		ft_putstr_fd(": No such file or directory\n", 2);
		ft_mem_cleanup();
		exit(1);
	}
	in = open(redir->filename, O_RDONLY);
	if (in < 0)
	{
		ft_mem_cleanup();
		exit(1);
	}
	dup2(in, STDIN_FILENO);
	close(in);
	return (0);
}

int	handle_redir_out(t_redir *redir)
{
	int	out;

	out = open(redir->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (out < 0)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(redir->filename, 2);
		ft_putstr_fd(": No such file or directory\n", 2);
		ft_mem_cleanup();
		exit(1);
	}
	dup2(out, STDOUT_FILENO);
	close(out);
	return (0);
}

int	handle_redir_append(t_redir *redir)
{
	int	out;

	out = open(redir->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (out < 0)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(redir->filename, 2);
		ft_putstr_fd(": No such file or directory\n", 2);
		ft_mem_cleanup();
		exit(1);
	}
	dup2(out, STDOUT_FILENO);
	close(out);
	return (0);
}

void	setup_heredoc_stdin(t_cmd *cmd, int *input_redirected)
{
	t_heredoc	*h;
	int			hpipe[2];

	if (!cmd->heredocs)
		return ;
	h = cmd->heredocs;
	while (h->next)
		h = h->next;
	if (h->content && !*input_redirected)
	{
		if (pipe(hpipe) == 0)
		{
			write(hpipe[1], h->content, ft_strlen(h->content));
			close(hpipe[1]);
			dup2(hpipe[0], STDIN_FILENO);
			close(hpipe[0]);
			*input_redirected = 1;
		}
	}
}

void	setup_redir_list(t_cmd *cmd, int *input_redirected,
		int *output_redirected)
{
	t_redir	*r;

	if (!cmd->redirs)
		return ;
	r = cmd->redirs;
	while (r)
	{
		if (r->type == REDIR_IN)
		{
			handle_redir_in(r);
			*input_redirected = 1;
		}
		else if (r->type == REDIR_OUT)
		{
			handle_redir_out(r);
			*output_redirected = 1;
		}
		else if (r->type == REDIR_APPEND)
		{
			handle_redir_append(r);
			*output_redirected = 1;
		}
		r = r->next;
	}
}
