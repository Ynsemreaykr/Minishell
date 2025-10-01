/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   apply_redir_builtin.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/29 02:50:41 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/09/10 10:21:22 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include <unistd.h>
#include <stdio.h>

int	handle_heredoc_redir_builtin(t_redir *r, int *input_redirected)
{
	int	hpipe[2];

	if (!r->content)
		return (0);
	if (pipe(hpipe) != 0)
		return (-1);
	write(hpipe[1], r->content, ft_strlen(r->content));
	close(hpipe[1]);
	dup2(hpipe[0], STDIN_FILENO);
	close(hpipe[0]);
	*input_redirected = 1;
	return (0);
}

int	redir_in_builtin(t_redir *r, int *input_redirected)
{
	int	infd;

	infd = open(r->filename, O_RDONLY);
	if (infd < 0)
	{
		perror(r->filename);
		return (1);
	}
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
	{
		perror(r->filename);
		return (1);
	}
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
	{
		perror(r->filename);
		return (1);
	}
	dup2(*outfd, STDOUT_FILENO);
	close(*outfd);
	return (0);
}
