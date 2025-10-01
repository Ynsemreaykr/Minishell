/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 15:54:07 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/09/10 10:21:31 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include <stdio.h>
#include <unistd.h>

static void	trim_space(const char *str, const char **start, const char **end)
{
	int	i;

	i = 0;
	while (str[i] && (str[i] == ' ' || str[i] == '\t' || str[i] == '\n'))
		i++;
	*start = &str[i];
	*end = str + ft_strlen(str) - 1;
	while (*end >= *start && (**end == ' ' || **end == '\t' || **end == '\n'))
		(*end)--;
}

static int	is_valid_number(const char *str)
{
	int			i;
	const char	*start;
	const char	*end;

	if (!str || !*str)
		return (0);
	trim_space(str, &start, &end);
	if (!*start)
		return (0);
	i = 0;
	if (start[i] == '+' || start[i] == '-')
		i++;
	if (start + i > end)
		return (0);
	while (start + i <= end)
	{
		if (start[i] < '0' || start[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

int	ft_exit(char **argv)
{
	int	arg_count;
	int	code;

	ft_putstr_fd("exit\n", 1);
	arg_count = 0;
	while (argv[arg_count])
		arg_count++;
	if (arg_count == 1)
		return (1);
	if (!is_valid_number(argv[1]))
	{
		ft_putstr_fd("minishell: exit: ", 2);
		ft_putstr_fd(argv[1], 2);
		ft_putstr_fd(": numeric argument required\n", 2);
		exit(2);
	}
	if (arg_count > 2)
	{
		ft_putstr_fd("minishell: exit: too many arguments\n", 2);
		return (1);
	}
	code = ft_atoi(argv[1]);
	ft_mem_cleanup();
	exit(code);
}

int	ft_pwd(t_shell *shell)
{
	char	*pwd_env;
	char	cwd[1024];

	pwd_env = get_env_var("PWD", shell);
	if (pwd_env)
	{
		ft_putstr_fd(pwd_env, 1);
		ft_putstr_fd("\n", 1);
	}
	else
	{
		if (getcwd(cwd, sizeof(cwd)))
		{
			ft_putstr_fd(cwd, 1);
			ft_putstr_fd("\n", 1);
		}
		else
			perror("pwd");
	}
	return (0);
}

int	is_builtin(const char *cmd)
{
	if (!cmd || ft_strlen(cmd) == 0)
		return (1);
	if (ft_strcmp(cmd, "echo") == 0)
		return (1);
	if (ft_strcmp(cmd, "exit") == 0)
		return (1);
	if (ft_strcmp(cmd, "pwd") == 0)
		return (1);
	if (ft_strcmp(cmd, "env") == 0)
		return (1);
	if (ft_strcmp(cmd, "cd") == 0)
		return (1);
	if (ft_strcmp(cmd, "export") == 0)
		return (1);
	if (ft_strcmp(cmd, "unset") == 0)
		return (1);
	if (ft_strcmp(cmd, ".") == 0)
		return (1);
	return (0);
}
