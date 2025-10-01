/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 10:47:05 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/09/10 10:23:05 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include <readline/readline.h>
#include <readline/history.h>

sig_atomic_t	g_signal_number = 0;

static t_shell	init_shell(char **envp)
{
	t_shell	shell;

	shell.last_exit = 0;
	shell.env = NULL;
	init_env(envp, &shell);
	update_shlvl(&shell);
	setup_normal_signals();
	return (shell);
}

static int	handle_input(char *input, t_shell *shell)
{
	if (!input)
		return (0);
	if (!ft_strcmp(input, "exit"))
	{
		ft_putstr_fd("exit\n", 1);
		ft_free(input);
		return (0);
	}
	if (!check_input(input, shell))
	{
		ft_free(input);
		return (1);
	}
	return (2);
}

static char	*get_user_input(void)
{
	char	*input;

	input = readline("minishell$ ");
	if (!input)
	{
		ft_putstr_fd("exit\n", 1);
		return (NULL);
	}
	if (*input)
		add_history(input);
	return (input);
}

static t_shell	setup_shell_and_get_input(int argc, char **argv, char **envp)
{
	t_shell	shell;

	if (argc > 1)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(argv[1], 2);
		ft_putstr_fd(": cannot execute binary file\n", 2);
		exit(126);
	}
	shell = init_shell(envp);
	return (shell);
}

int	main(int argc, char **argv, char **envp)
{
	t_shell	shell;
	char	*input;
	int		result;
	int		cmd_result;

	shell = setup_shell_and_get_input(argc, argv, envp);
	while (1)
	{
		input = get_user_input();
		result = handle_input(input, &shell);
		if (result == 0)
			break ;
		else if (result == 1)
			continue ;
		else if (result == 2)
		{
			cmd_result = process_command_from_input(input, &shell);
			if (cmd_result == 0)
				continue ;
			ft_free(input);
		}
	}
	cleanup_shell(&shell);
	return (shell.last_exit);
}
