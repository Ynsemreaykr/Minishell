/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/29 02:44:05 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/09/10 10:23:37 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

extern sig_atomic_t	g_signal_number;

void	handle_sigpipe(int signo)
{
	g_signal_number = signo;
	ft_mem_cleanup();
	exit(141);
}

void	setup_normal_signals(void)
{
	signal(SIGINT, normal_signal_handler);
	signal(SIGQUIT, handle_sigquit);
}

void	setup_heredoc_signals(void)
{
	signal(SIGINT, heredoc_signal_handler);
	signal(SIGQUIT, SIG_IGN);
}

void	setup_command_signals(void)
{
	signal(SIGINT, command_signal_handler);
	signal(SIGQUIT, handle_sigquit);
	signal(SIGPIPE, handle_sigpipe);
}

void	reset_signal_state(void)
{
	g_signal_number = 0;
}
