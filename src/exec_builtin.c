#include "../include/minishell.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/unistd.h>



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

static int	ft_echo(char ** argv)
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



int exec_builtin(t_cmd *cmd, t_shell *shell)
{ // arg için null check eklenebikir duruma göre
    if (ft_strcmp(cmd->argv[0], "echo") == 0)
        return ft_echo(cmd->argv);
    if (ft_strcmp(cmd->argv[0], "exit") == 0)
        return ft_exit(cmd->argv);
    if (ft_strcmp(cmd->argv[0], "pwd") == 0)
        return ft_pwd(cmd->argv);
    if (ft_strcmp(cmd->argv[0], "env") == 0)
        return ft_env(shell);
    if (ft_strcmp(cmd->argv[0], "cd") == 0)
        return ft_cd(cmd->argv, shell);
    if (ft_strcmp(cmd->argv[0], "export") == 0)
        return ft_export(cmd->argv, shell);
    if (ft_strcmp(cmd->argv[0], "unset") == 0)
        return ft_unset(cmd->argv, shell);
    return 1;
}

int	ft_export(char **argv, t_shell *shell)
{
	int	i;
	int	res;

	i = 1;
	if (!argv[i])
		return (handle_no_args_export(shell));
	res = 0;
	while (argv[i])
	{
		if (process_single_export_arg(argv[i], shell) != 0)
			res = 1;
		i++;
	}
	return (res);
}