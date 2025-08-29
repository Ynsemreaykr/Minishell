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
#include <string.h>
#include <unistd.h>

static int	is_n_flag(const char *arg)
{
	int	i;

	if (!arg || arg[0] != '-')
		return (0);
	i = 1;
	while (arg[i])
	{
		if (arg[i] != 'n')
			return (0);
		i++;
	}
	return (1);
}

static void	print_echo_args(char **argv, int start)
{
	int	i;

	i = start;
	while (argv[i])
	{
		printf("%s", argv[i]);
		if (argv[i + 1])
			printf(" ");
		i++;
	}
}

int	ft_echo(char **argv)
{
	int	n_flag;
	int	i;

	n_flag = 0;
	i = 1;
	while (argv[i] && is_n_flag(argv[i]))
	{
		n_flag = 1;
		i++;
	}
	print_echo_args(argv, i);
	if (!n_flag)
		printf("\n");
	return (0);
}

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
	fprintf(stderr, "ft_exit\n");
	exit(code);
}

int	ft_pwd(char **argv)
{
	char	cwd[1024];

	(void)argv;
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
