/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 02:45:12 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/09/10 10:22:24 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include <unistd.h>

static void	apply_single_redir_child(t_redir *r, int *input_redirected)
{
	if (r->type == REDIR_IN)
		apply_in_redir_child(r, input_redirected);
	else if (r->type == REDIR_OUT)
		apply_out_trunc_child(r);
	else if (r->type == REDIR_APPEND)
		apply_out_append_child(r);
	else if (r->type == HEREDOC)
		apply_heredoc_child(r, input_redirected);
}

static void	apply_all_redirs(t_redir *redir_list, int *input_redirected)
{
	t_redir	*r;

	r = redir_list;
	while (r)
	{
		apply_single_redir_child(r, input_redirected);
		r = r->next;
	}
}

void	setup_redirections_for_child(t_cmd *cmd, int fd_in, int *pipefd)
{
	int	input_redirected;

	input_redirected = 0;
	if (cmd->redirs)
		apply_all_redirs(cmd->redirs, &input_redirected);
	if (!input_redirected && fd_in != STDIN_FILENO)
	{
		dup2(fd_in, STDIN_FILENO);
		close(fd_in);
	}
	else if (fd_in != STDIN_FILENO)
		close(fd_in);
	if (cmd->next)
	{
		close(pipefd[0]);
		close(pipefd[1]);
	}
}
