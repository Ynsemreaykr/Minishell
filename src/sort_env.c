/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort_env.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 02:45:12 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/09/10 10:23:39 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

void	sort_env(char **env, int count)
{
	int		j;
	int		k;
	char	*tmp;

	j = 0;
	while (j < count - 1)
	{
		k = 0;
		while (k < count - j - 1)
		{
			if (ft_strcmp(env[k], env[k + 1]) > 0)
			{
				tmp = env[k];
				env[k] = env[k + 1];
				env[k + 1] = tmp;
			}
			k++;
		}
		j++;
	}
}

static void	print_single_env(char *entry)
{
	char	*eq;

	eq = ft_strchr(entry, '=');
	if (eq)
	{
		*eq = '\0';
		if (ft_strcmp(entry, "_") != 0)
		{
			ft_putstr_fd("declare -x ", 1);
			ft_putstr_fd(entry, 1);
			ft_putstr_fd("=\"", 1);
			ft_putstr_fd(eq + 1, 1);
			ft_putstr_fd("\"\n", 1);
		}
		*eq = '=';
	}
	else if (ft_strcmp(entry, "_") != 0)
	{
		ft_putstr_fd("declare -x ", 1);
		ft_putstr_fd(entry, 1);
		ft_putstr_fd("\n", 1);
	}
}

void	print_sorted_env(char **env, int count)
{
	int	j;

	j = 0;
	while (j < count)
	{
		print_single_env(env[j]);
		j++;
	}
}
