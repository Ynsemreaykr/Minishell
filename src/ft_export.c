/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_export.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 16:23:20 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/09/10 10:22:37 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

int	is_valid_identifier(const char *name)
{
	int	i;

	if (!name[0] || (!ft_isalpha(name[0]) && name[0] != '_'))
		return (0);
	i = 1;
	while (name[i])
	{
		if (!ft_isalnum(name[i]) && name[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

char	*extract_var_name(const char *arg)
{
	char	*eq;
	char	*name;
	int		len;

	eq = ft_strchr(arg, '=');
	if (eq)
	{
		len = eq - arg;
		name = ft_malloc(len + 1);
		ft_strncpy(name, arg, len);
		name[len] = '\0';
	}
	else
		name = ft_strdup(arg);
	return (name);
}

int	handle_no_args_export(t_shell *shell)
{
	char	**env;
	char	**sorted_env;
	int		count;
	int		j;

	env = get_env(shell);
	count = 0;
	while (env && env[count])
		count++;
	sorted_env = ft_malloc(sizeof(char *) * (count + 1));
	j = 0;
	while (j < count)
	{
		sorted_env[j] = ft_strdup(env[j]);
		j++;
	}
	sorted_env[count] = NULL;
	sort_env(sorted_env, count);
	print_sorted_env(sorted_env, count);
	j = 0;
	while (j < count)
		ft_free(sorted_env[j++]);
	ft_free(sorted_env);
	return (0);
}
