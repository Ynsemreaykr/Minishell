/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 16:02:53 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/09/10 10:21:41 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

void	cleanup_heredoc_content(t_cmd *cmds)
{
	t_cmd	*current;
	t_redir	*redir;

	current = cmds;
	while (current)
	{
		if (current->redirs)
		{
			redir = current->redirs;
			while (redir)
			{
				if (redir->type == HEREDOC && redir->content)
				{
					ft_free(redir->content);
					redir->content = NULL;
				}
				redir = redir->next;
			}
		}
		current = current->next;
	}
}

void	cleanup_env(t_shell *shell)
{
	int	i;

	if (!shell)
		return ;
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
	if (!shell)
		return ;
	cleanup_env(shell);
	ft_mem_cleanup();
}

static void	free_cmd_struct(t_cmd *cmd)
{
	int	i;

	if (!cmd)
		return ;
	if (cmd->argv)
	{
		i = 0;
		while (cmd->argv[i])
			ft_free(cmd->argv[i++]);
		ft_free(cmd->argv);
	}
	if (cmd->redirs)
		free_redir_list(cmd->redirs);
	ft_free(cmd);
}

void	cleanup_and_return_null(t_cmd *cmd, char **cmd_strings, int cmd_count)
{
	int	i;

	if (cmd)
		free_cmd_struct(cmd);
	if (cmd_strings)
	{
		i = 0;
		while (i < cmd_count)
			ft_free(cmd_strings[i++]);
		ft_free(cmd_strings);
	}
}
