/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 17:15:37 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/09/10 10:23:03 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

static int	process_command(t_cmd *cmds, t_shell *shell)
{
	if (!cmds)
		return (1);
	return (execute_command_main(cmds, shell));
}

static int	handle_empty_string_cmd(t_cmd *cmds,
				const char *input, t_shell *shell)
{
	int	i;

	i = 0;
	while (input[i] && (input[i] == ' ' || input[i] == '\t'))
		i++;
	if (input[i] == '"' && input[i + 1] == '"')
	{
		ft_putstr_fd("command not found\n", 2);
		shell->last_exit = 127;
	}
	else
		shell->last_exit = 0;
	free_cmds(cmds);
	return (1);
}

static int	handle_empty_or_invalid_cmd(t_cmd *cmds,
				const char *input, t_shell *shell)
{
	int	i;

	if (!cmds)
	{
		i = 0;
		while (input[i] && (input[i] == ' ' || input[i] == '\t'))
			i++;
		if (!input[i])
			shell->last_exit = 0;
		else
			shell->last_exit = 2;
		return (1);
	}
	if (!cmds->argv && !cmds->redirs)
	{
		free_cmds(cmds);
		return (1);
	}
	if (cmds->argv && !cmds->argv[0] && !cmds->redirs)
		return (handle_empty_string_cmd(cmds, input, shell));
	return (0);
}

static int	handle_special_cases(t_cmd *cmds, t_shell *shell)
{
	if (cmds->argv && cmds->argv[0] && ft_strlen(cmds->argv[0]) == 0)
	{
		ft_putstr_fd("minishell: : command not found\n", 2);
		shell->last_exit = 127;
		free_cmds(cmds);
		return (1);
	}
	if (cmds->argv && cmds->argv[0] && cmds->argv[1]
		&& !ft_strcmp(cmds->argv[0], "./minishell")
		&& !ft_strcmp(cmds->argv[1], "./minishell"))
	{
		ft_putstr_fd("./minishell: ./minishell: cannot execute ", 2);
		ft_putstr_fd("binary file\n", 2);
		shell->last_exit = 126;
		free_cmds(cmds);
		return (1);
	}
	return (0);
}

int	process_command_from_input(const char *input, t_shell *shell)
{
	t_cmd	*cmds;
	int		result;

	cmds = parse_commands(input, shell);
	if (handle_empty_or_invalid_cmd(cmds, input, shell))
		return (0);
	if (handle_special_cases(cmds, shell))
		return (0);
	result = process_command(cmds, shell);
	free_cmds(cmds);
	return (result);
}
