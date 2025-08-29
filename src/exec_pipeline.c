/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_pipeline.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 17:01:02 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/08/28 17:12:12 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include <sys/unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>

static void	child_setup(t_cmd *cmd, int fd_in, int *pipefd)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	if (cmd->next)
	{
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);
		close(pipefd[0]);
	}
	if (fd_in != 0)
	{
		dup2(fd_in, STDIN_FILENO);
		close(fd_in);
	}
	setup_redirections_for_child(cmd, fd_in, pipefd);
}

static void	handle_child(t_cmd *cmd, int fd_in, int *pipefd, t_shell *shell)
{
	child_setup(cmd, fd_in, pipefd);
	setup_command_signals();

	if (!cmd->argv || !cmd->argv[0])
	{
		cleanup_shell_for_child(shell);
		ft_mem_cleanup();
		exit(0);
	}
	if (is_builtin(cmd->argv[0]))
	{
		int exit_code = exec_builtin(cmd, shell);
		cleanup_shell_for_child(shell);
		ft_mem_cleanup();
		exit(exit_code);
	}
	else
	{
		execute_command(cmd, shell->env);
		cleanup_shell_for_child(shell);
		ft_mem_cleanup();
		exit(127);
	}
}

static int	handle_parent(int fd_in, int *pipefd, t_cmd *cmd)
{
	if (fd_in != 0)
		close(fd_in);
	if (cmd->next)
	{
		close(pipefd[1]);
		fd_in = pipefd[0];
	}
	return (fd_in);
}

static int	wait_and_cleanup(t_cmd *cmds)
{
	int	result;

	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	result = wait_for_children();
	cleanup_heredoc_content(cmds);
	setup_normal_signals();
	return (result);
}

int	exec_pipeline(t_cmd *cmds, t_shell *shell)
{
	int		fd_in;
	int		pipefd[2];
	pid_t	pid;
	t_cmd	*cmd;

	fd_in = 0;
	cmd = cmds;
	setup_command_signals();
	while (cmd)
	{
		if (cmd->next)
			pipe(pipefd);
		pid = create_child_process();
		if (pid == 0)
			handle_child(cmd, fd_in, pipefd, shell);
		else if (pid > 0)
			fd_in = handle_parent(fd_in, pipefd, cmd);
		cmd = cmd->next;
	}
	return (wait_and_cleanup(cmds));
}
