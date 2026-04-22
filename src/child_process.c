/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   child_process.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 08:21:36 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/10 10:21:37 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Komut için gereken fork mantığını standartlaştıran ve çocuk işlemlerin statüsünü
** yakalayarak asıl çıkış kodunu çıkaran modül. */

#include "../include/minishell.h"
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>

/* Normal standart bir fork çağrısı atar ve işlem hatasında uyarı vererek
** -1 döner. Başarılı fork da PID döner. */
pid_t	create_child_process(void)
{
	pid_t	pid;

	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		return (-1);
	}
	return (pid);
}

/* Pipeline'da yakalanmış son çocuğun statüsünü alır ve
** dışarıdan bir sinyal ile mi öldü (örnek: Ctrl+C yani SIGINT 130)
** yoksa doğal yoldan hata veya başarıyla bitirip exit ile mi çıktı bakar.
** Buna mukabil shell in genel return değeri için uygun çıkış kodunu ayarlar. */
static int	get_exit_code(int last_status)
{
	int	sig;

	if (WIFSIGNALED(last_status))
	{
		sig = WTERMSIG(last_status);
		if (sig == SIGINT)
		{
			write(STDOUT_FILENO, "\n", 1);
			return (130);
		}
	}
	if (WIFEXITED(last_status))
		return (WEXITSTATUS(last_status));
	return (last_status);
}

/* Pipe segmentleri ardışık başlatıldığından parent tüm forkları beklemelidir.
** Arka planda ölen tüm alt işlemleri süpürür ve bizim listemizin son çocuğunu
** (binaenaleyh asıl ekrana yansıyacak çıkış değerinin de sahibi) tespit ederek
** ona ait doğru çıkış kodunu döndürür. */
int	wait_for_pipeline(pid_t last_pid)
{
	int		status;
	pid_t	pid;
	int		last_status;

	last_status = 0;
	pid = wait(&status);
	while (pid > 0)
	{
		if (pid == last_pid)
			last_status = status;
		pid = wait(&status);
	}
	setup_normal_signals();
	return (get_exit_code(last_status));
}
