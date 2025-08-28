/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 17:15:37 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/08/28 17:23:15 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <readline/readline.h>
#include <fcntl.h>
#include <sys/wait.h>

static int	heredoc_read(int *pipefd, t_heredoc *heredoc)
{
	char	*content;

	close(pipefd[1]);
	content = read_heredoc_content(pipefd[0]);
	close(pipefd[0]);
	if (content == NULL)
		return (-1);
	if (heredoc->content)
		ft_free(heredoc->content);
	heredoc->content = content;
	return (0);
}

static int	heredoc_parent_process(pid_t pid, int *pipefd, t_heredoc *heredoc)
{
	int	status;
	int	read_result;

	read_result = heredoc_read(pipefd, heredoc);
	waitpid(pid, &status, 0);
	setup_normal_signals();
	if (read_result == -1)
		return (-1);
	if (WIFEXITED(status) && WEXITSTATUS(status) == 130)
	{
		if (heredoc->content)
		{
			ft_free(heredoc->content);
			heredoc->content = NULL;
		}
		return (130);
	}
	return (0);
}

static void	write_heredoc_line(int fd, char *line)
{
	write(fd, line, ft_strlen(line));
	write(fd, "\n", 1);
}

static void	process_heredoc_input(int *pipefd,
	t_heredoc *heredoc, t_shell *shell)
{
	char	*line;
	char	*expanded_line;

	while (1)
	{
		line = readline("> ");
		if (g_signal_number || !line
			|| ft_strcmp(line, heredoc->cleaned_delimiter) == 0)
		{
			if (!line && !g_signal_number)
				printf("minishell: here-document delimited by end-of-file\n");
			ft_free(line);
			break ;
		}
		if (heredoc->quoted_flag != 0)
			expanded_line = ft_strdup(line);
		else
			expanded_line = expand_variable_in_heredoc(line, shell);
		if (expanded_line)
		{
			write_heredoc_line(pipefd[1], expanded_line);
			ft_free(expanded_line);
		}
		ft_free(line);
	}
}

static void	heredoc_child_process(int *pipefd,
	t_heredoc *heredoc, t_shell *shell)
{
	signal(SIGINT, heredoc_interrupt_handler);
	signal(SIGQUIT, SIG_IGN);
	close(pipefd[0]);
	process_heredoc_input(pipefd, heredoc, shell);
	close(pipefd[1]);
	if (g_signal_number)
		cleanup_and_exit(130);
	cleanup_and_exit(0);
}

int	process_single_heredoc(t_heredoc *heredoc, t_shell *shell)
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
		heredoc_child_process(pipefd, heredoc, shell);
	else
		return (heredoc_parent_process(pid, pipefd, heredoc));
	return (0);
}
