/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_expand.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 09:40:57 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/09/10 10:22:45 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

static int	handle_quote_processing(const char *line, int i,
					int *in_quote, char *quote_char)
{
	if ((line[i] == '"' || line[i] == '\'') && !(*in_quote))
	{
		*quote_char = line[i];
		*in_quote = 1;
		return (1);
	}
	else if (line[i] == *quote_char && *in_quote)
	{
		*in_quote = 0;
		*quote_char = 0;
		return (1);
	}
	return (0);
}

static int	expand_exit_code(t_shell *shell, char *expanded, int *i)
{
	char	*exit_str;
	int		len;

	exit_str = ft_itoa(shell->last_exit);
	len = 0;
	if (exit_str)
	{
		ft_strcpy(expanded, exit_str);
		len = ft_strlen(exit_str);
		ft_free(exit_str);
	}
	(*i)++;
	return (len);
}

static int	process_variable_expansion(const char *line, int *i,
						t_shell *shell, char *expanded)
{
	int		var_start;
	int		added_len;

	var_start = *i;
	while (line[*i] && (ft_isalnum(line[*i]) || line[*i] == '_'))
		(*i)++;
	added_len = expand_environment_variable(line, var_start, shell, expanded);
	return (added_len);
}

static int	dollar_expansion(const char *line, int *i,
					t_shell *shell, char *expanded)
{
	int	added_len;

	added_len = 0;
	(*i)++;
	if (line[*i] == '?')
		added_len = expand_exit_code(shell, expanded, i);
	else if (ft_isalnum(line[*i]) || line[*i] == '_')
		added_len = process_variable_expansion(line, i, shell, expanded);
	else
	{
		expanded[0] = '$';
		added_len = 1;
	}
	return (added_len);
}

void	process_expansion_loop(const char *line, t_shell *shell, char *expanded)
{
	char	quote_char;
	int		exp_pos;
	int		i;
	int		in_quote;

	exp_pos = 0;
	i = 0;
	quote_char = 0;
	while (line[i])
	{
		if (quote_char != 0)
			in_quote = 1;
		else
			in_quote = 0;
		if (handle_quote_processing(line, i, &in_quote, &quote_char))
			expanded[exp_pos++] = line[i++];
		else if (line[i] == '$' && (quote_char != '\''))
			exp_pos += dollar_expansion(line, &i, shell, expanded + exp_pos);
		else
			expanded[exp_pos++] = line[i++];
	}
	expanded[exp_pos] = '\0';
}
