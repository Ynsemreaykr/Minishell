/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 15:54:07 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/08/28 15:59:40 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

	




static int	is_valid_number(const char *str)
{
	int			i;
	const char	*start;
	const char	*end;

	i = 0;
	start = str;
	end = str + ft_strlen(str) - 1;
	if (start[i] == '+' || start[i] == '-')
		i++;
	if (!start[i])
		return (0);
	while (start[i] && start + i <= end)
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
	exit(code);
}

int	ft_pwd()
{
	char	cwd[1024];

	if (getcwd(cwd, sizeof(cwd)))
		printf("%s\n", cwd);
	else
		perror("pwd");
	return (0);
}

int	is_builtin(const char *cmd)
{
	if (!cmd)
		return (0);
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
	return (0);
}
