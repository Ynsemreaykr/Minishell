/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 16:02:53 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/08/28 16:05:20 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

void	cleanup_heredoc_content(t_cmd *cmds)
{
	t_cmd		*current;
	t_heredoc	*heredoc;

	current = cmds;
	while (current)
	{
		if (current->heredocs)
		{
			heredoc = current->heredocs;
			while (heredoc)
			{
				if (heredoc->content)
				{
					ft_free(heredoc->content);
					heredoc->content = NULL;
				}
				heredoc = heredoc->next;
			}
		}
		current = current->next;
	}
}

void	cleanup_env(t_shell *shell)
{
	int	i;

	if (shell->env)
	{
		i = 0;
		while (shell->env[i])
		{
			ft_free(shell->env[i]);
			i++;
		}
		ft_free(shell->env);
		shell->env = NULL;
	}
}

void	cleanup_shell_for_child(t_shell *shell)
{
	cleanup_env(shell);
	if (shell->last_arg)
	{
		ft_free(shell->last_arg);
		shell->last_arg = NULL;
	}
}

void	cleanup_and_return_null(t_cmd *cmd,
			char **cmd_strings, int cmd_count)
{
	int	i;

	if (cmd)
	{
		if (cmd->argv)
		{
			i = 0;
			while (cmd->argv[i])
			{
				ft_free(cmd->argv[i]);
				i++;
			}
			ft_free(cmd->argv);
		}
		if (cmd->heredocs)
			free_heredoc_list(cmd->heredocs);
		if (cmd->redirs)
			free_redir_list(cmd->redirs);
		ft_free(cmd);
	}
	i = 0;
	while (i < cmd_count)
		ft_free(cmd_strings[i++]);
	ft_free(cmd_strings);
}
