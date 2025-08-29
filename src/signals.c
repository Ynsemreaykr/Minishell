/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/29 02:44:05 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/08/29 02:46:14 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <signal.h>
#include <stdio.h>
#include <readline/readline.h>
#include <unistd.h>
#include <sys/wait.h>
#include "../include/minishell.h"

extern volatile sig_atomic_t	g_signal_number;

void	normal_signal_handler(int signo)
{
	g_signal_number = signo;
	write(STDOUT_FILENO, "\n", 1);
	rl_replace_line("", 0);
	rl_on_new_line();
	rl_redisplay();
}

void	heredoc_signal_handler(int signo)
{
	(void)signo;
	g_signal_number = 1;
	write(STDOUT_FILENO, "\n", 1);
	close(STDIN_FILENO);
}

void	heredoc_interrupt_handler(int signum)
{
	(void)signum;
	g_signal_number = 1;
	write(STDOUT_FILENO, "\n", 1);
	close(STDIN_FILENO);
}

void	command_signal_handler(int signo)
{
	g_signal_number = signo;
	write(STDOUT_FILENO, "\n", 1);
}

void	handle_sigquit(int signo)
{
	g_signal_number = signo;
}
	
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

int	check_signal_status(void)
{
	if (g_signal_number == SIGINT)
		return (130);
	else if (g_signal_number == SIGQUIT)
		return (131);
	return (0);
}
