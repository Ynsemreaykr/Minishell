/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipeline.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 17:01:02 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/09/10 10:22:18 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
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
	int	exit_code;

	child_setup(cmd, fd_in, pipefd);
	setup_command_signals();
	if (!cmd->argv || !cmd->argv[0])
	{
		cleanup_shell_for_child(shell);
		exit(0);
	}
	if (is_builtin(cmd->argv[0]))
	{
		exit_code = exec_builtin(cmd, shell);
		cleanup_shell_for_child(shell);
		exit(exit_code);
	}
	else
	{
		execute_command(cmd, shell->env);
		cleanup_shell_for_child(shell);
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

static pid_t	pipeline_commands(t_cmd *cmds, t_shell *shell, int *fd_in_ptr)
{
	int		pipefd[2];
	pid_t	pid;
	pid_t	last_pid;
	t_cmd	*cmd;

	cmd = cmds;
	last_pid = -1;
	while (cmd)
	{
		if (cmd->next)
			pipe(pipefd);
		pid = create_child_process();
		if (pid == 0)
			handle_child(cmd, *fd_in_ptr, pipefd, shell);
		else if (pid > 0)
		{
			if (!cmd->next)
				last_pid = pid;
			*fd_in_ptr = handle_parent(*fd_in_ptr, pipefd, cmd);
		}
		cmd = cmd->next;
	}
	return (last_pid);
}

int	exec_pipeline(t_cmd *cmds, t_shell *shell)
{
	int		fd_in;
	pid_t	last_pid;

	fd_in = 0;
	setup_command_signals();
	last_pid = pipeline_commands(cmds, shell, &fd_in);
	cleanup_heredoc_content(cmds);
	return (wait_for_pipeline(last_pid));
}
