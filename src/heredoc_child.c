/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_child.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 02:45:12 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/10 10:22:43 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Heredoc child process modülü.
** Fork sonrası child process'te readline ile satır satır giriş okur
** ve pipe yazma ucuna yazar. */

#include "../include/minishell.h"
#include <unistd.h>
#include <readline/readline.h>

extern volatile sig_atomic_t	g_signal_number;

/* Heredoc satırındaki değişkenleri ($VAR, $?) genişletir.
** calculate_expansion_size ile gereken tampon boyutu hesaplanır,
** process_expansion_loop ile genişletilmiş string oluşturulur. */
static char	*expand_variable_in_heredoc(const char *line, t_shell *shell)
{
	char	*expanded;
	int		required_size;

	required_size = calculate_expansion_size(line, shell);
	expanded = ft_malloc(required_size + 1);
	process_expansion_loop(line, shell, expanded);
	return (expanded);
}

/* İşlenmiş bir heredoc satırını pipe yazma ucuna yazar.
** Satırın kendisi + "\n" yazılır, ardından line serbest bırakılır. */
static void	write_heredoc_line(int fd, char *line)
{
	write(fd, line, ft_strlen(line));
	write(fd, "\n", 1);
	ft_free(line);
}

/* Kullanıcıdan heredoc satırlarını readline ile okur.
** "> " prompt'u ile her satır alınır.
** Döngü şu durumlarda sonlanır:
** - g_signal_number (Ctrl+C) alındıysa
** - EOF (Ctrl+D) → uyarı mesajı
** - Delimiter ile eşleşen satır
** quoted_flag != 0 ise değişken genişletme yapılmaz (tırnaklı delimiter).
** Aksi hâlde expand_variable_in_heredoc çağrılır. */
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

/* Child process heredoc giriş döngüsü giriş noktası.
** 1. Sinyal kurulumu: SIGINT → heredoc_signal_handler, SIGQUIT → SIG_IGN
** 2. Okuma ucu kapatılır (child yalnızca yazma yapar)
** 3. process_heredoc_input ile satır loop çalışır
** 4. Yazma ucu kapatılır
** 5. g_signal_number != 0 ise exit(130), aksi hâlde exit(0) */
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
