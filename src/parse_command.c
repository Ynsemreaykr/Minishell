/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_command.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 17:36:54 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/09/10 10:23:11 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

static void	cleanup_cmd_strings(char **cmd_strings, int cmd_count)
{
	int	c;

	c = 0;
	while (c < cmd_count)
		ft_free(cmd_strings[c++]);
	ft_free(cmd_strings);
}

static char	**get_cmd_string(const char *input, int *cmd_count)
{
	char	**cmd_strings;

	cmd_strings = split_by_pipes(input, cmd_count);
	if (!cmd_strings)
	{
		ft_putstr_fd(
			"minishell: syntax error near unexpected token `|'\n", 2);
		return (NULL);
	}
	return (cmd_strings);
}

t_cmd	*parse_commands(const char *input, t_shell *shell)
{
	t_cmd		*head;
	t_cmd		*last;
	t_cmd_data	data;
	int			cmd_count;
	char		**cmd_strings;

	if (!validate_input_and_pipes(input))
		return (NULL);
	head = NULL;
	last = NULL;
	cmd_strings = get_cmd_string(input, &cmd_count);
	if (!cmd_strings)
		return (NULL);
	data.cmd_strings = cmd_strings;
	data.cmd_count = cmd_count;
	data.shell = shell;
	if (!process_cmd_loop(&data, &head, &last))
		return (NULL);
	cleanup_cmd_strings(cmd_strings, cmd_count);
	return (head);
}

int	is_builtin_command(const char *cmd)
{
	if (!cmd)
		return (0);
	if (ft_strcmp(cmd, "echo") == 0
		|| ft_strcmp(cmd, "cd") == 0
		|| ft_strcmp(cmd, "pwd") == 0
		|| ft_strcmp(cmd, "export") == 0
		|| ft_strcmp(cmd, "unset") == 0
		|| ft_strcmp(cmd, "env") == 0
		|| ft_strcmp(cmd, "exit") == 0)
		return (1);
	return (0);
}
