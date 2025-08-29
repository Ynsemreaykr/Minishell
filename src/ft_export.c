/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_export.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 16:23:20 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/08/28 16:26:52 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

#include "../include/minishell.h"
#include <stdio.h>
#include <string.h>

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
		name = ft_malloc(len + 1, __FILE__, __LINE__);
		ft_strncpy(name, arg, len);
		name[len] = '\0';
	}
	else
		name = ft_strdup(arg);
	return (name);
}

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

void	print_sorted_env(char **env, int count)
{
	int		j;
	char	*eq;

	j = 0;
	while (j < count)
	{
		eq = ft_strchr(env[j], '=');
		if (eq)
		{
			*eq = '\0';
			if (ft_strcmp(env[j], "_") != 0)
				printf("declare -x %s=\"%s\"\n", env[j], eq + 1);
			*eq = '=';
		}
		else if (ft_strcmp(env[j], "_") != 0)
			printf("declare -x %s\n", env[j]);
		j++;
	}
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
	sorted_env = ft_malloc(sizeof(char *) * (count + 1), __FILE__, __LINE__);
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

int	process_single_export_arg(char *arg, t_shell *shell)
{
	char	*var_name;
	char	*eq;

	var_name = extract_var_name(arg);
	eq = ft_strchr(arg, '=');
	if (!is_valid_identifier(var_name))
	{
		ft_putstr_fd("minishell: export: `", 2);
		ft_putstr_fd(arg, 2);
		ft_putstr_fd("': not a valid identifier\n", 2);
		ft_free(var_name);
		return (1);
	}
	if (eq)
	{
		*eq = '\0';
		set_env_var(var_name, eq + 1, shell);
		*eq = '=';
	}
	else if (!get_env_var(var_name, shell))
		set_env_var(var_name, NULL, shell);
	ft_free(var_name);
	return (0);
}

