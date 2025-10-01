/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 17:15:37 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/09/10 10:22:50 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include <sys/wait.h>
#include <unistd.h>

static int	heredoc_read(int *pipefd, t_redir *heredoc_redir)
{
	char	*content;

	close(pipefd[1]);
	content = read_heredoc_content(pipefd[0]);
	close(pipefd[0]);
	if (content == NULL)
		return (-1);
	if (heredoc_redir->content)
		ft_free(heredoc_redir->content);
	heredoc_redir->content = content;
	return (0);
}

static int	parent_process(pid_t pid, int *pipefd, t_redir *heredoc_redir)
{
	int	status;
	int	read_result;

	read_result = heredoc_read(pipefd, heredoc_redir);
	waitpid(pid, &status, 0);
	setup_normal_signals();
	if (read_result == -1)
		return (-1);
	if (WIFEXITED(status) && WEXITSTATUS(status) == 130)
	{
		if (heredoc_redir->content)
		{
			ft_free(heredoc_redir->content);
			heredoc_redir->content = NULL;
		}
		return (130);
	}
	return (0);
}

int	process_single_heredoc(t_redir *heredoc_redir, t_shell *shell)
{
	int		pipefd[2];
	pid_t	pid;

	if (pipe(pipefd) == -1)
		return (-1);
	signal(SIGINT, SIG_IGN);
	pid = fork();
	if (pid == -1)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		return (-1);
	}
	if (pid == 0)
		heredoc_child_process(pipefd, heredoc_redir, shell);
	else
		return (parent_process(pid, pipefd, heredoc_redir));
	return (0);
}
