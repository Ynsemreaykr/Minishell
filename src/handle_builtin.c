/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_builtin.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 16:57:02 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/08/28 17:00:26 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include <fcntl.h>

static void	restore_builtin_redirections(int old_stdin, int old_stdout)
{
	if (old_stdout != -1)
	{
		dup2(old_stdout, STDOUT_FILENO);
		close(old_stdout);
	}
	if (old_stdin != -1)
	{
		dup2(old_stdin, STDIN_FILENO);
		close(old_stdin);
	}
}

static int	setup_builtin_redirections(t_cmd *cmd, int *outfd)
{
	int		error_count;
	int		input_redirected;
	int		output_redirected;
	t_redir	*current;

	error_count = 0;
	input_redirected = 0;
	output_redirected = 0;
	redirect_heredoc_for_builtin(cmd, &input_redirected);
	current = cmd->redirs;
	while (current)
	{
		if (current->type == REDIR_IN)
			error_count += redir_in_builtin(current, &input_redirected);
		else if (current->type == REDIR_OUT)
			error_count
				+= redir_out_builtin(current, &output_redirected, outfd);
		else if (current->type == REDIR_APPEND)
			error_count
				+= redir_append_builtin(current, &output_redirected, outfd);
		current = current->next;
	}
	return (error_count);
}

int	exec_builtin_with_redirections(t_cmd *cmd, t_shell *shell)
{
	int	old_stdin;
	int	old_stdout;
	int	outfd;
	int	redir_errors;
	int	result;

	old_stdin = dup(STDIN_FILENO);
	old_stdout = dup(STDOUT_FILENO);
	outfd = -1;
	setup_command_signals();
	redir_errors = setup_builtin_redirections(cmd, &outfd);
	if (redir_errors > 0)
		result = 1;
	else
		result = exec_builtin(cmd, shell);
	restore_builtin_redirections(old_stdin, old_stdout);
	setup_normal_signals();
	return (result);
}
