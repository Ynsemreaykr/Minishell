/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_parser.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 15:49:44 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/08/28 15:49:45 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include <string.h>
#include <stdlib.h>

static void	toggle_quote_state(char c,
				int *in_single_quote, int *in_double_quote)
{
	if (c == '\'' && !*in_double_quote)
		*in_single_quote = !*in_single_quote;
	else if (c == '"' && !*in_single_quote)
		*in_double_quote = !*in_double_quote;
}

static int	count_pipes(const char *input)
{
	int	pipe_count;
	int	i;
	int	in_single_quote;
	int	in_double_quote;

	pipe_count = 0;
	i = 0;
	in_single_quote = 0;
	in_double_quote = 0;
	while (input[i])
	{
		toggle_quote_state(input[i], &in_single_quote, &in_double_quote);
		if (input[i] == '|' && !in_single_quote && !in_double_quote)
			pipe_count++;
		i++;
	}
	return (pipe_count);
}

static void	add_command_to_array(char **commands, int *cmd_count,
				const char *input, int start, int end)
{
	int	len;

	if (end > start)
	{
		len = end - start;
		commands[*cmd_count] = ft_malloc(len + 1, __FILE__, __LINE__);
		ft_strncpy(commands[*cmd_count], input + start, len);
		commands[*cmd_count][len] = '\0';
		(*cmd_count)++;
	}
}

static void	parse_pipe_loop(char **commands, int *cmd_count,
				const char *input, int *start, int *i)
{
	int	in_single_quote;
	int	in_double_quote;

	in_single_quote = 0;
	in_double_quote = 0;
	while (input[*i])
	{
		toggle_quote_state(input[*i], &in_single_quote, &in_double_quote);
		if (input[*i] == '|' && !in_single_quote && !in_double_quote)
		{
			add_command_to_array(commands, cmd_count, input, *start, *i);
			*start = (*i) + 1;
		}
		(*i)++;
	}
}

char	**split_by_pipes(const char *input, int *count)
{
	int		pipe_count;
	int		max_commands;
	char	**commands;
	int		cmd_count;
	int		start;
	int		i;

	pipe_count = count_pipes(input);
	max_commands = pipe_count + 1;
	commands = ft_malloc(sizeof(char *) * (max_commands + 1),
			__FILE__, __LINE__);
	cmd_count = 0;
	start = 0;
	i = 0;
	parse_pipe_loop(commands, &cmd_count, input, &start, &i);
	add_command_to_array(commands, &cmd_count, input, start, i);
	commands[cmd_count] = NULL;
	*count = cmd_count;
	return (commands);
}
