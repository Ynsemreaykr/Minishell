/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/29 00:03:35 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/09/10 10:21:44 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include <readline/readline.h>

void	free_args(char **args)
{
	int	i;

	i = 0;
	if (!args)
		return ;
	while (args[i])
	{
		ft_free(args[i]);
		i++;
	}
	ft_free(args);
}

void	free_cmds(t_cmd *cmd)
{
	t_cmd	*tmp;

	while (cmd)
	{
		tmp = cmd->next;
		free_args(cmd->argv);
		if (cmd->redirs)
			free_redir_list(cmd->redirs);
		ft_free(cmd);
		cmd = tmp;
	}
}

void	free_redir_list(t_redir *redirs)
{
	t_redir	*current;
	t_redir	*next;

	current = redirs;
	while (current)
	{
		next = current->next;
		if (current->filename)
			ft_free(current->filename);
		if (current->delimiter)
			ft_free(current->delimiter);
		if (current->cleaned_delimiter)
			ft_free(current->cleaned_delimiter);
		if (current->content)
			ft_free(current->content);
		ft_free(current);
		current = next;
	}
}

void	cleanup_shell(t_shell *shell)
{
	cleanup_env(shell);
	reset_signal_state();
	rl_clear_history();
	ft_mem_cleanup();
}
