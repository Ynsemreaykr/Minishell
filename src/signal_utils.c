/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/29 02:44:05 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/10 10:23:34 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Sinyal işleyici fonksiyonlarını içeren modül.
** signals.c'deki kurulum fonksiyonlarına register edilen
** gerçek handler fonksiyonları burada tanımlanır. */

#include "../include/minishell.h"
#include <unistd.h>
#include <readline/readline.h>

/* Dışarıdan erişilen global sinyal değişkeni. */
extern sig_atomic_t	g_signal_number;

/* Normal mod SIGINT (Ctrl+C) işleyicisi.
** - Sinyal numarasını g_signal_number'a kaydeder.
** - Ekrana yeni satır yazar.
** - Readline prompt satırını temizler ve yeniden gösterir.
** Kullanıcı prompt beklerken Ctrl+C basınca yeni boş prompt gelir. */
void	normal_signal_handler(int signo)
{
	g_signal_number = signo;
	write(STDOUT_FILENO, "\n", 1);
	rl_replace_line("", 0);
	rl_on_new_line();
	rl_redisplay();
}

/* Heredoc okuma modu SIGINT işleyicisi.
** - g_signal_number = 1 (kesme sinyali alındı işareti)
** - Ekrana yeni satır yazar.
** - STDIN_FILENO'yu kapatır → readline döngüsü sonlanır,
**   heredoc_child_process() 130 ile çıkar. */
void	heredoc_signal_handler(int signo)
{
	(void)signo;
	g_signal_number = 1;
	write(STDOUT_FILENO, "\n", 1);
	close(STDIN_FILENO);
}

/* Komut çalıştırma modu SIGINT işleyicisi.
** - Sinyal numarasını g_signal_number'a kaydeder.
** - Ekrana yeni satır yazar.
** Child process'ler kendi default handler'larıyla sonlanır;
** parent burada sadece sinyal aldığını not eder. */
void	command_signal_handler(int signo)
{
	g_signal_number = signo;
	write(STDOUT_FILENO, "\n", 1);
}

/* SIGQUIT (Ctrl+\) işleyicisi.
** Sinyal numarasını kaydeder ancak herhangi bir çıktı üretmez.
** Child process'ler default SIGQUIT davranışıyla core dump üretir. */
void	handle_sigquit(int signo)
{
	g_signal_number = signo;
}
