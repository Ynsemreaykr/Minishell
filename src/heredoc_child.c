/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_child.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 02:45:12 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/09/10 10:22:43 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include <unistd.h>
#include <readline/readline.h>

extern volatile sig_atomic_t	g_signal_number;

static char	*expand_variable_in_heredoc(const char *line, t_shell *shell)
{
	char	*expanded;
	int		required_size;

	required_size = calculate_expansion_size(line, shell);
	expanded = ft_malloc(required_size + 1);
	process_expansion_loop(line, shell, expanded);
	return (expanded);
}

static void	write_heredoc_line(int fd, char *line)
{
	write(fd, line, ft_strlen(line));
	write(fd, "\n", 1);
	ft_free(line);
}

static void	process_heredoc_input(int *pipefd,
	t_redir *heredoc_redir, t_shell *shell)
{
	char	*line;
	char	*expanded_line;

	while (1)
	{
		line = readline("> ");
		if (g_signal_number || !line
			|| ft_strcmp(line, heredoc_redir->cleaned_delimiter) == 0)
		{
			if (!line && !g_signal_number)
			{
				ft_putstr_fd("minishell: here-document delimited by "
					"end-of-file\n", 2);
			}
			ft_free(line);
			break ;
		}
		if (heredoc_redir->quoted_flag != 0)
			expanded_line = ft_strdup(line);
		else
			expanded_line = expand_variable_in_heredoc(line, shell);
		if (expanded_line)
			write_heredoc_line(pipefd[1], expanded_line);
	}
}

void	heredoc_child_process(int *pipefd,
	t_redir *heredoc_redir, t_shell *shell)
{
	signal(SIGINT, heredoc_signal_handler);
	signal(SIGQUIT, SIG_IGN);
	close(pipefd[0]);
	process_heredoc_input(pipefd, heredoc_redir, shell);
	close(pipefd[1]);
	if (g_signal_number)
	{
		ft_mem_cleanup();
		exit(130);
	}
	ft_mem_cleanup();
	exit(0);
}
