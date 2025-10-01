/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   apply_redir_child.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 02:45:12 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/09/10 10:21:26 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include <unistd.h>
#include <stdio.h>

void	apply_in_redir_child(t_redir *r, int *input_redirected)
{
	int	infd;

	infd = open(r->filename, O_RDONLY);
	if (infd >= 0)
	{
		dup2(infd, STDIN_FILENO);
		close(infd);
		*input_redirected = 1;
	}
	else
	{
		perror(r->filename);
		ft_mem_cleanup();
		exit(1);
	}
}

void	apply_out_trunc_child(t_redir *r)
{
	int	out;

	out = open(r->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (out >= 0)
	{
		dup2(out, STDOUT_FILENO);
		close(out);
	}
	else
	{
		perror(r->filename);
		ft_mem_cleanup();
		exit(1);
	}
}

void	apply_out_append_child(t_redir *r)
{
	int	out;

	out = open(r->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (out >= 0)
	{
		dup2(out, STDOUT_FILENO);
		close(out);
	}
	else
	{
		perror(r->filename);
		ft_mem_cleanup();
		exit(1);
	}
}

void	apply_heredoc_child(t_redir *r, int *input_redirected)
{
	int	hpipe[2];

	if (!r->content)
		return ;
	if (pipe(hpipe) == 0)
	{
		write(hpipe[1], r->content, ft_strlen(r->content));
		close(hpipe[1]);
		dup2(hpipe[0], STDIN_FILENO);
		close(hpipe[0]);
		*input_redirected = 1;
	}
}
