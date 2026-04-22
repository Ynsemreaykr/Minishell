/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/29 02:44:05 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/10 10:23:37 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Sinyal kurulum fonksiyonlarını içeren modül.
** Shell üç farklı modda farklı sinyal davranışları kullanır:
**   - Normal mod (prompt bekleme)
**   - Heredoc okuma modu
**   - Komut çalıştırma modu */

#include "../include/minishell.h"

/* Dışarıdan erişilen global sinyal değişkeni (main.c'de tanımlı). */
extern sig_atomic_t	g_signal_number;

/* SIGPIPE sinyali işleyicisi.
** Pipe bağlantısı koptuğunda çağrılır (örn. "yes | head -1").
** Sinyal numarasını g_signal_number'a kaydeder,
** tüm belleği temizler ve 141 çıkış kodu ile sonlanır. */
void	handle_sigpipe(int signo)
{
	g_signal_number = signo;
	ft_mem_cleanup();
	exit(141);
}

/* Normal mod sinyal kurulumunu yapar (prompt bekleme sırasında):
** - SIGINT (Ctrl+C) → normal_signal_handler (yeni satır + prompt yenile)
** - SIGQUIT (Ctrl+\) → handle_sigquit (sinyal numarasını kaydet, sessiz) */
void	setup_normal_signals(void)
{
	signal(SIGINT, normal_signal_handler);
	signal(SIGQUIT, handle_sigquit);
}

/* Heredoc okuma modu sinyal kurulumunu yapar:
** - SIGINT (Ctrl+C) → heredoc_signal_handler (stdin kapat, okumayı kes)
** - SIGQUIT (Ctrl+\) → yoksay (heredoc sırasında Ctrl+\ etkisiz) */
void	setup_heredoc_signals(void)
{
	signal(SIGINT, heredoc_signal_handler);
	signal(SIGQUIT, SIG_IGN);
}

/* Komut çalıştırma modu sinyal kurulumunu yapar:
** - SIGINT (Ctrl+C) → command_signal_handler (yeni satır yaz, kaydeder)
** - SIGQUIT (Ctrl+\) → handle_sigquit
** - SIGPIPE → handle_sigpipe (pipe kopunca 141 ile çık) */
void	setup_command_signals(void)
{
	signal(SIGINT, command_signal_handler);
	signal(SIGQUIT, handle_sigquit);
	signal(SIGPIPE, handle_sigpipe);
}

/* Global sinyal durumunu sıfırlar (0'a çeker).
** Shell'in bir sonraki komuta temiz durumla başlaması için çağrılır. */
void	reset_signal_state(void)
{
	g_signal_number = 0;
}
