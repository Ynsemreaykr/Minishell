/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 15:33:45 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/08/28 15:46:30 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

static char	*create_env_string(const char *name, const char *value)
{
	int		name_len;
	int		value_len;
	char	*env_str;

	if (value)
	{
		name_len = ft_strlen(name);
		value_len = ft_strlen(value);
		env_str = ft_malloc(name_len + value_len + 2, __FILE__, __LINE__);
		if (!env_str)
			return (NULL);
		ft_strcpy(env_str, name);
		ft_strcat(env_str, "=");
		ft_strcat(env_str, value);
		return (env_str);
	}
	env_str = ft_malloc(ft_strlen(name) + 1, __FILE__, __LINE__);
	if (!env_str)
		return (NULL);
	ft_strcpy(env_str, name);
	return (env_str);
}

static int	add_new_env_var(const char *name, const char *value, t_shell *shell)
{
	int		count;
	int		i;
	char	**new_env;

	count = 0;
	while (shell->env[count])
		count++;
	new_env = ft_malloc(sizeof(char *) * (count + 2), __FILE__, __LINE__);
	if (!new_env)
		return (-1);
	i = 0;
	while (i < count)
	{
		new_env[i] = shell->env[i];
		i++;
	}
	new_env[count] = create_env_string(name, value);
	new_env[count + 1] = NULL;
	ft_free(shell->env);
	shell->env = new_env;
	return (0);
}

int	set_env_var(const char *name, const char *value, t_shell *shell)
{
	int	i;
	int	name_len;
	int	existing_index;

	if (!name)
		return (-1);
	name_len = ft_strlen(name);
	existing_index = -1;
	i = -1;
	while (shell->env[++i])
	{
		if ((shell->env[i][name_len] == '=' || shell->env[i][name_len] == '\0')
			&& ft_strncmp(shell->env[i], name, name_len) == 0)
		{
			existing_index = i;
			break ;
		}
	}
	if (existing_index != -1)
	{
		ft_free(shell->env[existing_index]);
		shell->env[existing_index] = create_env_string(name, value);
		return (0);
	}
	return (add_new_env_var(name, value, shell));
}

int	unset_env_var(const char *name, t_shell *shell)
{
	int	name_len;
	int	i;
	int	j;

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
			j = i;
			while (shell->env[j])
			{
				shell->env[j] = shell->env[j + 1];
				j++;
			}
			return (0);
		}
		i++;
	}
	return (-1);
}
