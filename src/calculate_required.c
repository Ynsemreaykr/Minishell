/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   calculate_required.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 02:45:12 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/09/10 10:21:34 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

static int	get_max_env_variable_size(t_shell *shell)
{
	char	**envp;
	int		max_size;
	int		i;
	char	*equal_pos;

	envp = get_env(shell);
	max_size = 0;
	i = 0;
	if (!envp)
		return (0);
	while (envp[i])
	{
		equal_pos = ft_strchr(envp[i], '=');
		if (equal_pos)
		{
			if ((int)ft_strlen(equal_pos + 1) > max_size)
				max_size = ft_strlen(equal_pos + 1);
		}
		i++;
	}
	return (max_size);
}

static int	calculate_token_memory_size(int base_len,
				int has_variable_expansion,
				int has_single_quotes,
				t_shell *shell)
{
	int	max_env_size;

	if (has_single_quotes && !has_variable_expansion)
		return (base_len + 1);
	else if (has_variable_expansion)
	{
		max_env_size = get_max_env_variable_size(shell);
		if (max_env_size == 0)
			return (base_len + 1);
		return (base_len + max_env_size);
	}
	else
		return (base_len + 1);
}

int	calculate_required_memory(const char *input, int start, int end,
				t_shell *shell)
{
	int	base_len;
	int	has_variable_expansion;
	int	has_single_quotes;
	int	i;

	base_len = end - start;
	has_variable_expansion = 0;
	has_single_quotes = 0;
	i = start;
	while (i < end)
	{
		if (input[i] == '$' && i + 1 < end
			&& (ft_isalnum(input[i + 1]) || input[i + 1] == '_'
				|| input[i + 1] == '?'))
			has_variable_expansion = 1;
		if (input[i] == '\'')
			has_single_quotes = 1;
		i++;
	}
	return (calculate_token_memory_size(base_len,
			has_variable_expansion, has_single_quotes, shell));
}
