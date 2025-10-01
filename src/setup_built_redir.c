/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup_built_redir.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/29 02:50:41 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/09/10 10:23:32 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

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
	else if (r->type == HEREDOC)
	{
		if (handle_heredoc_redir_builtin(r, input_redirected) == -1)
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
