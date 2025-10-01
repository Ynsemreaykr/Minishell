/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_check_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/29 00:03:35 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/09/10 10:22:53 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

static int	count_redir(const char *input, int *i, char redir_char)
{
	int	count;

	count = 0;
	while (input[*i] == redir_char)
	{
		count++;
		(*i)++;
	}
	return (count);
}

static int	validate_redir(char redir_char, int redir_count,
		const char *input, int i)
{
	if (redir_count > 2)
	{
		ft_putstr_fd(
			"minishell: syntax error near unexpected token `newline'\n", 2);
		return (0);
	}
	if ((redir_count == 2 && redir_char == '<' && input[i] == '>')
		|| (redir_count == 2 && redir_char == '>' && input[i] == '<'))
	{
		ft_putstr_fd(
			"minishell: syntax error near unexpected token `", 2);
		ft_putstr_fd("newline'\n", 2);
		return (0);
	}
	return (1);
}

int	check_redir_operator(const char *input, int *i)
{
	char	redir_char;
	int		redir_count;
	int		j;

	redir_char = input[*i];
	redir_count = count_redir(input, i, redir_char);
	if (!validate_redir(redir_char, redir_count, input, *i))
		return (0);
	j = *i;
	while (input[j] == ' ' || input[j] == '\t')
		j++;
	if (input[j] == '|')
	{
		ft_putstr_fd(
			"minishell: syntax error near unexpected token `|'\n", 2);
		return (0);
	}
	return (1);
}
