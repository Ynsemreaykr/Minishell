/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipeline.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 17:01:02 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/10 10:22:18 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Pipe ("|") operatörüyle birbirine zincirlenen komutların fork(), pipe() ve dup2()
** kullanılarak birbirlerine bağlanması ve parallel yürütülmesi işlemlerini
** koordine eden modüldür. */

#include "../include/minishell.h"
#include <unistd.h>

/* Pipeline üzerindeki herbir child processin (çocuğun) file descriptor atamasını yapar.
** Sinyalleri (Ctrl C vs) pipe içinde Default haline çeker.
** cmd->next (sonraki komut) varsa, onun pipe ının yazma ucuna kendi standart
** çıktısını (STDOUT) bağlar. (Kendi çıktısını sonrakinin stdin ine aktaracak).
** fd_in doluysa, kendi standart girdisini öncekilerin aktardığı bu degere setler. */
static void	child_setup(t_cmd *cmd, int fd_in, int *pipefd)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	if (cmd->next)
	{
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);
		close(pipefd[0]);
	}
	if (fd_in != 0)
	{
		dup2(fd_in, STDIN_FILENO);
		close(fd_in);
	}
	setup_redirections_for_child(cmd, fd_in, pipefd);
}

/* Pipe segmentindeki bir komutun (child process) tamamen çalıştırıldığı noktadır.
** fd (yönlendirme) ayarlamalarını child_setup a devrtettikten sonra
** komutun builtin olup olmadığına bakar, dahiliyse process içinde execte eder,
** hariciyse execute_command() ile fork üzerinden execute ettirir. */
static void	handle_child(t_cmd *cmd, int fd_in, int *pipefd, t_shell *shell)
{
	int	exit_code;

	child_setup(cmd, fd_in, pipefd);
	setup_command_signals();
	if (!cmd->argv || !cmd->argv[0])
	{
		cleanup_shell_for_child(shell);
		exit(0);
	}
	if (is_builtin(cmd->argv[0]))
	{
		exit_code = exec_builtin(cmd, shell);
		cleanup_shell_for_child(shell);
		exit(exit_code);
	}
	else
	{
		execute_command(cmd, shell->env);
		cleanup_shell_for_child(shell);
		exit(127);
	}
}

/* Her fork'tan sonra parent process in yapacağı dosya/fd temizliğidir.
** Pipe ın yazma ucu kapatılır, okuma ucu (fd_in) bir sonraki komutun kullanabilmesi
** için sonraki iterasyona aktarılır. */
static int	handle_parent(int fd_in, int *pipefd, t_cmd *cmd)
{
	if (fd_in != 0)
		close(fd_in);
	if (cmd->next)
	{
		close(pipefd[1]);
		fd_in = pipefd[0];
	}
	return (fd_in);
}

/* cmd dizisi (linked listi) üzerindeki teker teker her bir pipe objesine komuta
** ait fork() ve pipe() oluşturup çalıştırma döngüsüdür.
** pidleri döndürür, dizideki en sonuncu çocuğun PID numarasını return eder. */
static pid_t	pipeline_commands(t_cmd *cmds, t_shell *shell, int *fd_in_ptr)
{
	int		pipefd[2];
	pid_t	pid;
	pid_t	last_pid;
	t_cmd	*cmd;

	cmd = cmds;
	last_pid = -1;
	while (cmd)
	{
		if (cmd->next)
			pipe(pipefd);
		pid = create_child_process();
		if (pid == 0)
			handle_child(cmd, *fd_in_ptr, pipefd, shell);
		else if (pid > 0)
		{
			if (!cmd->next)
				last_pid = pid;
			*fd_in_ptr = handle_parent(*fd_in_ptr, pipefd, cmd);
		}
		cmd = cmd->next;
	}
	return (last_pid);
}

/* Pipeline ana fonksiyonudur. Commandler birbirlerine pipe yapılmışsa (cmds->next!=NULL)
** burası devreye girer. Pipeline oluşturulup bitirildikten ve statüler
** (return code) alındıktan sonra artık heredoc içeriği temizlenir ve çıkış alınır. */
int	exec_pipeline(t_cmd *cmds, t_shell *shell)
{
	int		fd_in;
	pid_t	last_pid;

	fd_in = 0;
	setup_command_signals();
	last_pid = pipeline_commands(cmds, shell, &fd_in);
	cleanup_heredoc_content(cmds);
	return (wait_for_pipeline(last_pid));
}
