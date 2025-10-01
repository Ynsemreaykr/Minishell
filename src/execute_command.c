/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_command.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 17:01:02 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/09/10 10:22:21 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include <sys/stat.h>
#include <unistd.h>

static int	is_accessable(char *command, char **splitted_path, char **full_path)
{
	struct stat	path_stat;

	if (ft_strchr(command, '/'))
	{
		if (stat(command, &path_stat) == 0 && S_ISDIR(path_stat.st_mode))
			return (-3);
		if (access(command, F_OK) != 0)
			return (-4);
		if (access(command, X_OK) != 0)
			return (-2);
		*full_path = ft_strdup(command);
		return (0);
	}
	return (search_in_path(command, splitted_path, full_path));
}

static int	get_exec_error_exit_code(int res, const char *cmd0)
{
	if (res == -3)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(cmd0, 2);
		ft_putstr_fd(": Is a directory\n", 2);
		return (126);
	}
	else if (res == -4)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(cmd0, 2);
		ft_putstr_fd(": No such file or directory\n", 2);
		return (127);
	}
	return (-1);
}

static void	exec_error_and_exit(int res, const char *cmd0)
{
	int	exit_code;

	exit_code = get_exec_error_exit_code(res, cmd0);
	if (exit_code == -1)
	{
		if (res == -2)
		{
			ft_putstr_fd("minishell: ", 2);
			ft_putstr_fd(cmd0, 2);
			ft_putstr_fd(": Permission denied\n", 2);
			exit_code = 126;
		}
		else
		{
			ft_putstr_fd("minishell: ", 2);
			ft_putstr_fd(cmd0, 2);
			ft_putstr_fd(": command not found\n", 2);
			exit_code = 127;
		}
	}
	ft_mem_cleanup();
	exit(exit_code);
}

static void	try_execve(char *full_path, t_cmd *cmd, char **envp)
{
	execve(full_path, cmd->argv, envp);
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(cmd->argv[0], 2);
	ft_putstr_fd(": command not found\n", 2);
	ft_free(full_path);
	ft_mem_cleanup();
	exit(127);
}

void	execute_command(t_cmd *cmd, char **envp)
{
	char	**splitted_path;
	char	*full_path;
	int		result;

	splitted_path = parse_path(envp);
	full_path = NULL;
	result = is_accessable(cmd->argv[0], splitted_path, &full_path);
	if (splitted_path)
		ft_split_free(splitted_path);
	if (result != 0)
		exec_error_and_exit(result, cmd->argv[0]);
	try_execve(full_path, cmd, envp);
}
