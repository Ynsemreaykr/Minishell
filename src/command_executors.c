/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_executors.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 16:07:32 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/10 10:33:15 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Tek komut (pipe içermeyen) yapıların dahili/harici (builtin/external)
** olarak çalıştırılmasını ve fork() işlemlerini idare eden modül. */

#include "../include/minishell.h"
#include <sys/wait.h>
#include <unistd.h>

/* Komutun dahili komutlardan biri (echo, cd, export vs.) olup olmadığını
** kontrol ederek ilgili komut fonksiyonunu çağırır.
** Bilinmeyen dahililer (ft_dot vb.) hatalarla sonuçlanabilir. */
int	exec_builtin(t_cmd *cmd, t_shell *shell)
{
	if (ft_strcmp(cmd->argv[0], "echo") == 0)
		return (ft_echo(cmd->argv));
	if (ft_strcmp(cmd->argv[0], "exit") == 0)
		return (ft_exit(cmd->argv));
	if (ft_strcmp(cmd->argv[0], "pwd") == 0)
		return (ft_pwd(shell));
	if (ft_strcmp(cmd->argv[0], "env") == 0)
		return (ft_env(shell));
	if (ft_strcmp(cmd->argv[0], "cd") == 0)
		return (ft_cd(cmd->argv, shell));
	if (ft_strcmp(cmd->argv[0], "export") == 0)
		return (ft_export(cmd->argv, shell));
	if (ft_strcmp(cmd->argv[0], "unset") == 0)
		return (ft_unset(cmd->argv, shell));
	if (ft_strcmp(cmd->argv[0], ".") == 0)
		return (ft_dot(cmd->argv));
	return (1);
}

/* Pipe içermeyen TEK bir dahili komutun çalıştırılmasından sorumludur.
** Fork işlemi _yapılmaz_! Bu sayede (cd, export gibi) komutların
** shell ortamını kalıcı olarak değiştirmesi sağlanır.
** Orijinal stdin ve stdout saklanır, yönlendirmeler dup2() ile
** builtin öncesi ayarlanıp sonrasında eski haline getirilir. */
int	execute_single_builtin(t_cmd *cmd, t_shell *shell)
{
	int	old_stdin;
	int	old_stdout;
	int	result;

	old_stdin = dup(STDIN_FILENO);
	old_stdout = dup(STDOUT_FILENO);
	result = 0;
	if (setup_redirections_for_builtin(cmd) == -1)
		result = 1;
	else
		result = exec_builtin(cmd, shell);
	dup2(old_stdin, STDIN_FILENO);
	dup2(old_stdout, STDOUT_FILENO);
	close(old_stdin);
	close(old_stdout);
	return (result);
}

/* Parent process sinyalleri yoksayar, çocuğunu (pid) bekler (waitpid) ve
** onun nasıl sonlandığını (WIFEXITED / WIFSIGNALED) analiz edip
** çıkış kodunu döndürür. (Ctrl+C => 130, Ctrl+\ => 131 vs.) */
static int	handle_parent_process(pid_t pid)
{
	int	status;
	int	sig;

	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	waitpid(pid, &status, 0);
	setup_normal_signals();
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
	{
		sig = WTERMSIG(status);
		if (sig == SIGINT)
		{
			write(STDOUT_FILENO, "\n", 1);
			return (130);
		}
		else if (sig == SIGQUIT)
			return (131);
		return (128 + sig);
	}
	return (1);
}

/* Pipe içermeyen TEK dış (external) komutun (örn. ls, cat)
** fork ile child processte çalıştırılmasını başlatır.
** Child: Yönlendirmeleri kurup, sinyallere varsayılan değeri atayıp çalışır.
** Parent: process'i bekleyerek geri dönüş çıkış kodunu döndürür. */
int	execute_single_external(t_cmd *cmd, t_shell *shell)
{
	pid_t	pid;

	pid = create_child_process();
	if (pid == 0)
	{
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		setup_redirections_for_child(cmd, STDIN_FILENO, NULL);
		setup_command_signals();
		if (cmd->argv && cmd->argv[0] && ft_strlen(cmd->argv[0]) > 0)
			execute_command(cmd, shell->env);
		cleanup_shell_for_child(shell);
		exit(0);
	}
	else if (pid > 0)
	{
		return (handle_parent_process(pid));
	}
	return (1);
}
