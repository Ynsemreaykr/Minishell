/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 08:43:06 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/09/10 10:58:05 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include <unistd.h>

static void	add_basic_env_vars(t_shell *shell)
{
	char	cwd[1024];
	char	*existing_path;

	if (getcwd(cwd, sizeof(cwd)))
		set_env_var("PWD", cwd, shell);
	set_env_var("SHLVL", "0", shell);
	set_env_var("_", "./minishell", shell);
	existing_path = getenv("PATH");
	if (existing_path && *existing_path)
		set_env_var("PATH", existing_path, shell);
}

static void	copy_env_variable(char **env, char *envp_var, int index)
{
	int	len;

	len = ft_strlen(envp_var);
	env[index] = ft_malloc(len + 1);
	ft_strcpy(env[index], envp_var);
}

void	init_env(char **envp, t_shell *shell)
{
	int	count;
	int	i;

	if (!envp || !envp[0])
	{
		shell->env = ft_malloc(sizeof(char *) * 1);
		shell->env[0] = NULL;
		add_basic_env_vars(shell);
		return ;
	}
	count = 0;
	while (envp[count])
		count++;
	shell->env = ft_malloc(sizeof(char *) * (count + 1));
	i = 0;
	while (i < count)
	{
		copy_env_variable(shell->env, envp[i], i);
		i++;
	}
	shell->env[count] = NULL;
}

char	*get_env_var(const char *name, t_shell *shell)
{
	int	name_len;
	int	i;

	if (!shell->env || !name)
		return (NULL);
	name_len = ft_strlen(name);
	i = 0;
	while (shell->env[i])
	{
		if (ft_strncmp(shell->env[i], name, name_len) == 0
			&& shell->env[i][name_len] == '=')
			return (shell->env[i] + name_len + 1);
		i++;
	}
	return (NULL);
}

void	update_shlvl(t_shell *shell)
{
	char	*shlvl_str;
	char	*new_shlvl;
	int		shlvl_value;

	shlvl_str = get_env_var("SHLVL", shell);
	shlvl_value = 0;
	if (shlvl_str)
	{
		shlvl_value = ft_atoi(shlvl_str);
		if (shlvl_value < 0)
			shlvl_value = 0;
	}
	shlvl_value++;
	new_shlvl = ft_itoa(shlvl_value);
	set_env_var("SHLVL", new_shlvl, shell);
	ft_free(new_shlvl);
}
