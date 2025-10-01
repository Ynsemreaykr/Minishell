/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/29 02:44:05 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/09/10 10:23:34 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include <unistd.h>
#include <readline/readline.h>

extern sig_atomic_t	g_signal_number;

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

void	command_signal_handler(int signo)
{
	g_signal_number = signo;
	write(STDOUT_FILENO, "\n", 1);
}

void	handle_sigquit(int signo)
{
	g_signal_number = signo;
}
