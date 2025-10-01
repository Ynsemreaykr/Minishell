/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_unset.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 02:45:12 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/09/10 10:22:40 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

static void	shift_env_vars_left(char **env, int start_index)
{
	int	j;

	j = start_index;
	while (env[j])
	{
		env[j] = env[j + 1];
		j++;
	}
}

int	unset_env_var(const char *name, t_shell *shell)
{
	int	name_len;
	int	i;

	if (!shell->env || !name)
		return (-1);
	name_len = ft_strlen(name);
	i = 0;
	while (shell->env[i])
	{
		if ((shell->env[i][name_len] == '=' || shell->env[i][name_len] == '\0')
			&& ft_strncmp(shell->env[i], name, name_len) == 0)
		{
			ft_free(shell->env[i]);
			shift_env_vars_left(shell->env, i);
			return (0);
		}
		i++;
	}
	return (-1);
}

int	ft_unset(char **argv, t_shell *shell)
{
	int	i;

	i = 1;
	while (argv[i])
	{
		unset_env_var(argv[i], shell);
		i++;
	}
	return (0);
}
