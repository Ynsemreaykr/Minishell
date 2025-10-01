/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_env_content.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 02:45:12 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/09/10 10:22:28 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

static char	*find_env_variable(char *var_name, t_shell *shell)
{
	char	**envp;
	char	*var_value;
	int		i;
	int		name_len;

	envp = get_env(shell);
	var_value = NULL;
	name_len = ft_strlen(var_name);
	i = 0;
	while (envp && envp[i])
	{
		if (ft_strncmp(envp[i], var_name, name_len) == 0
			&& envp[i][name_len] == '=')
		{
			var_value = envp[i] + name_len + 1;
			break ;
		}
		i++;
	}
	return (var_value);
}

static int	copy_var_value(char *var_value, char *expanded, int exp_pos)
{
	int	len;

	if (var_value)
	{
		ft_strcpy(expanded + exp_pos, var_value);
		len = ft_strlen(var_value);
	}
	else
		len = 0;
	return (len);
}

int	expand_environment_variable(const char *line, int start,
					t_shell *shell, char *expanded)
{
	char	*var_name;
	char	*var_value;
	int		var_len;
	int		i;

	i = start;
	while (line[i] && (ft_isalnum(line[i]) || line[i] == '_'))
		i++;
	var_len = i - start;
	if (var_len == 0)
		return (0);
	var_name = ft_malloc(var_len + 1);
	ft_strncpy(var_name, line + start, var_len);
	var_name[var_len] = '\0';
	var_value = find_env_variable(var_name, shell);
	var_len = copy_var_value(var_value, expanded, 0);
	ft_free(var_name);
	return (var_len);
}
