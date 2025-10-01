/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_path.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 17:36:54 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/09/10 10:23:15 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include <unistd.h>

char	**parse_path(char **envp)
{
	int		i;
	char	*env_path;

	i = 0;
	env_path = NULL;
	while (envp[i])
	{
		if (!ft_strncmp(envp[i], "PATH=", 5))
		{
			env_path = envp[i] + 5;
			break ;
		}
		i++;
	}
	if (!env_path)
		return (NULL);
	return (ft_split(env_path, ':'));
}

int	search_in_path(char *command, char **splitted_path, char **full_path)
{
	int		i;
	char	*temp_path;

	if (!splitted_path)
		return (-1);
	i = 0;
	while (splitted_path[i])
	{
		temp_path = ft_strjoin(splitted_path[i], "/");
		*full_path = ft_strjoin(temp_path, command);
		ft_free(temp_path);
		if (access(*full_path, X_OK) == 0)
			return (0);
		ft_free(*full_path);
		i++;
	}
	*full_path = NULL;
	return (-1);
}
