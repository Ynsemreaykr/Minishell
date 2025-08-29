/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_executors.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 16:07:32 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/08/28 16:09:35 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

int	execute_single_builtin(t_cmd *cmd, t_shell *shell)
{
	int	old_stdin;
	int	old_stdout;
	int	result;

	old_stdin = dup(STDIN_FILENO);
	old_stdout = dup(STDOUT_FILENO);
	result = 0;
	if (setup_redirections_for_builtin(cmd) == -1)
		result = 1;
	else
		result = exec_builtin(cmd, shell);
	dup2(old_stdin, STDIN_FILENO);
	dup2(old_stdout, STDOUT_FILENO);
	close(old_stdin);
	close(old_stdout);
	return (result);
}

static int	handle_parent_process(pid_t pid)
{
	int	status;
	int	sig;

	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	waitpid(pid, &status, 0);
	if (WIFSIGNALED(status))
	{
		sig = WTERMSIG(status);
		if (sig == SIGINT)
			write(STDOUT_FILENO, "\n", 1);
	}
	setup_normal_signals();
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
	{
		sig = WTERMSIG(status);
		if (sig == SIGINT)
			return (130);
		else if (sig == SIGQUIT)
			return (131);
		return (128 + sig);
	}
	return (1);
}

int	execute_single_external(t_cmd *cmd, t_shell *shell)
{
	pid_t	pid;

	pid = create_child_process();
	if (pid == 0)
	{
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		setup_redirections_for_child(cmd, STDIN_FILENO, NULL);
		setup_command_signals();
		if (cmd->argv && cmd->argv[0])
			execute_command(cmd, shell->env);
		cleanup_shell_for_child(shell);
		ft_mem_cleanup();
		exit(0);
	}
	else if (pid > 0)
	{
		return (handle_parent_process(pid));
	}
	return (1);
}
