/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_check.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 17:36:54 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/09/10 10:23:18 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

static int	is_in_quotes(const char *input, int pos)
{
	int		i;
	int		in_quote;
	char	quote_char;

	i = 0;
	in_quote = 0;
	quote_char = 0;
	while (i <= pos)
	{
		if ((input[i] == '"' || input[i] == '\'') && !in_quote)
		{
			quote_char = input[i];
			in_quote = 1;
		}
		else if (input[i] == quote_char && in_quote)
		{
			in_quote = 0;
			quote_char = 0;
		}
		i++;
	}
	return (in_quote);
}

static int	pipe_check(const char *input)
{
	int		len;
	int		i;
	int		j;

	len = ft_strlen(input);
	i = 0;
	while (i < len && (input[i] == ' ' || input[i] == '\t'))
		i++;
	if (i < len && input[i] == '|')
		return (0);
	if (len > 0 && input[len - 1] == '|')
		return (0);
	i = -1;
	while (i++ < len - 1)
	{
		if (input[i] == '|' && !is_in_quotes(input, i))
		{
			j = i + 1;
			while (j < len && (input[j] == ' ' || input[j] == '\t'))
				j++;
			if (j < len && input[j] == '|')
				return (0);
		}
	}
	return (1);
}

int	count_pipes(const char *input)
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

int	validate_input_and_pipes(const char *input)
{
	if (!pipe_check(input))
	{
		ft_putstr_fd(
			"minishell: syntax error near unexpected token `|'\n", 2);
		return (0);
	}
	return (1);
}
