/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 17:15:37 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/10 10:22:50 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Heredoc okuma ana modülü.
** << operatörü için fork + pipe mekanizmasıyla satır satır okuma yapılır.
** Okunan içerik t_redir->content alanına kaydedilir. */

#include "../include/minishell.h"
#include <sys/wait.h>
#include <unistd.h>

/* Pipe'ın yazma ucunu kapatıp okuma ucundan heredoc içeriğini okur.
** read_heredoc_content ile tüm içerik tek string'e toplanır.
** Sonuç heredoc_redir->content alanına atanır.
** Okuma başarısızsa -1 döner. */
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

/* Heredoc fork'unda parent process yönetimi:
** 1. heredoc_read → pipe'tan içeriği okur
** 2. waitpid → child'ı bekler
** 3. Normal sinyal kurulumunu geri yükler
** 4. Child Ctrl+C ile kesilmişse (exit 130) içerik temizlenir, 130 döner
** Başarıda 0, okuma hatasında -1 döner. */
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

/* Tek bir heredoc kaydını işler (fork + pipe mekanizması):
** 1. pipe() ile iletişim kanalı oluşturulur
** 2. SIGINT parent için yoksayılır (child kendi handler'ını kurar)
** 3. fork():
**    - PID = 0 (child) → heredoc_child_process() (readline döngüsü)
**    - PID > 0 (parent) → parent_process() (okuma + bekleme)
** -1 → hata, 0 → başarı, 130 → Ctrl+C ile iptal */
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
