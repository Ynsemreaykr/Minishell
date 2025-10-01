/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_handlers.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 08:39:38 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/09/10 10:21:54 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

static int	handle_cmd_heredocs(t_cmd *cmd, t_shell *shell, t_cmd *all_cmds)
{
	t_redir	*redir;
	int		result;

	if (!cmd->redirs)
		return (0);
	redir = cmd->redirs;
	while (redir)
	{
		if (redir->type == HEREDOC)
		{
			result = process_single_heredoc(redir, shell);
			if (result != 0)
			{
				cleanup_heredoc_content(all_cmds);
				shell->last_exit = result;
				return (-1);
			}
		}
		redir = redir->next;
	}
	return (0);
}

static int	handle_all_heredocs(t_cmd *cmds, t_shell *shell)
{
	t_cmd	*current;
	int		result;

	current = cmds;
	while (current)
	{
		result = handle_cmd_heredocs(current, shell, cmds);
		if (result != 0)
			return (-1);
		current = current->next;
	}
	return (1);
}

static int	execute_single_command(t_cmd *cmd, t_shell *shell)
{
	int	result;

	if (cmd->argv && cmd->argv[0] && is_builtin(cmd->argv[0]))
	{
		result = execute_single_builtin(cmd, shell);
		shell->last_exit = result;
		if (result == 0)
			return (1);
		return (0);
	}
	result = execute_single_external(cmd, shell);
	shell->last_exit = result;
	if (result == 0)
		return (1);
	return (0);
}

int	execute_command_main(t_cmd *cmds, t_shell *shell)
{
	int	result;

	if (handle_all_heredocs(cmds, shell) == -1)
		return (-1);
	if (cmds->next)
	{
		result = exec_pipeline(cmds, shell);
		shell->last_exit = result;
		if (result == 0)
			return (1);
		return (0);
	}
	return (execute_single_command(cmds, shell));
}
