/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 02:45:12 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/10 10:22:24 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Komut çalıştırılacağı esnada child process (çocuk işlem) için
** yönlendirme (redirection) işlemlerini ayarlayan modül. */

#include "../include/minishell.h"
#include <unistd.h>

/* Tek bir yönlendirme tipi (IN, OUT, APPEND, HEREDOC) için 
** ilgili child redirection fonksiyonunu çağırır. Input redirect edildiyse
** bayrağı işaretler. */
static void	apply_single_redir_child(t_redir *r, int *input_redirected)
{
	if (r->type == REDIR_IN)
		apply_in_redir_child(r, input_redirected);
	else if (r->type == REDIR_OUT)
		apply_out_trunc_child(r);
	else if (r->type == REDIR_APPEND)
		apply_out_append_child(r);
	else if (r->type == HEREDOC)
		apply_heredoc_child(r, input_redirected);
}

/* Bir komut nesnesinin (cmd) barındırdığı tüm yönlendirme nesnelerini (redir_list)
** dolaşır ve her biri için dosyaları/pipeları açarak standart G/Ç'ye bağlar. */
static void	apply_all_redirs(t_redir *redir_list, int *input_redirected)
{
	t_redir	*r;

	r = redir_list;
	while (r)
	{
		apply_single_redir_child(r, input_redirected);
		r = r->next;
	}
}

/* Child process yaratıldıktan hemen sonra çağrılan, fd dup2 ayarlamalarını tek çatı
** altında toplayan ana fonksiyondur. Input bir yönlendirme dosyası tarafından
** manipüle edilmemişse, o halde pipe üzerinden fd_in e dup2lanır. */
void	setup_redirections_for_child(t_cmd *cmd, int fd_in, int *pipefd)
{
	int	input_redirected;

	input_redirected = 0;
	if (cmd->redirs)
		apply_all_redirs(cmd->redirs, &input_redirected);
	if (!input_redirected && fd_in != STDIN_FILENO)
	{
		dup2(fd_in, STDIN_FILENO);
		close(fd_in);
	}
	else if (fd_in != STDIN_FILENO)
		close(fd_in);
	if (cmd->next)
	{
		close(pipefd[0]);
		close(pipefd[1]);
	}
}
