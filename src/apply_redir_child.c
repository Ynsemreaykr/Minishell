/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   apply_redir_child.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 02:45:12 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/10 10:21:26 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Child process içinde yönlendirmelerin uygulandığı modül.
** dup2() sistem çağrısı ile dosya tanımlayıcıları yeniden bağlanır. */

#include "../include/minishell.h"
#include <unistd.h>
#include <stdio.h>

/* Giriş yönlendirmesi (< dosya): stdin'i dosyadan okuyacak şekilde ayarlar.
** - Dosya O_RDONLY ile açılır.
** - dup2(infd, STDIN_FILENO) → stdin artık dosyadan okur.
** - input_redirected = 1 → pipe'tan giriş alınmayacağını işaretler.
** - Dosya açılamazsa perror + exit(1). */
void	apply_in_redir_child(t_redir *r, int *input_redirected)
{
	int	infd;

	infd = open(r->filename, O_RDONLY);
	if (infd >= 0)
	{
		dup2(infd, STDIN_FILENO);
		close(infd);
		*input_redirected = 1;
	}
	else
	{
		perror(r->filename);
		ft_mem_cleanup();
		exit(1);
	}
}

/* Çıkış yönlendirmesi (> dosya): stdout'u dosyaya yönlendirir.
** O_WRONLY | O_CREAT | O_TRUNC bayrakları ile dosya açılır (sıfırdan başlar).
** dup2(out, STDOUT_FILENO) ile stdout dosyaya bağlanır.
** Dosya açılamazsa perror + exit(1). */
void	apply_out_trunc_child(t_redir *r)
{
	int	out;

	out = open(r->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (out >= 0)
	{
		dup2(out, STDOUT_FILENO);
		close(out);
	}
	else
	{
		perror(r->filename);
		ft_mem_cleanup();
		exit(1);
	}
}

/* Ekleme yönlendirmesi (>> dosya): stdout'u dosyanın sonuna yönlendirir.
** O_WRONLY | O_CREAT | O_APPEND bayrakları ile dosya açılır (sonuna eklenir).
** dup2(out, STDOUT_FILENO) → stdout dosya sonuna yönlendirilir.
** Dosya açılamazsa perror + exit(1). */
void	apply_out_append_child(t_redir *r)
{
	int	out;

	out = open(r->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (out >= 0)
	{
		dup2(out, STDOUT_FILENO);
		close(out);
	}
	else
	{
		perror(r->filename);
		ft_mem_cleanup();
		exit(1);
	}
}

/* Heredoc yönlendirmesi: önceden saklanan içeriği stdin'e bağlar.
** r->content NULL ise hiçbir şey yapılmaz.
** Aksi hâlde:
**   - pipe() oluşturulur
**   - İçerik pipe'ın yazma ucuna yazılır, yazma ucu kapatılır
**   - dup2(hpipe[0], STDIN_FILENO) → stdin okuma ucundan okur
**   - input_redirected = 1 olarak işaretlenir */
void	apply_heredoc_child(t_redir *r, int *input_redirected)
{
	int	hpipe[2];

	if (!r->content)
		return ;
	if (pipe(hpipe) == 0)
	{
		write(hpipe[1], r->content, ft_strlen(r->content));
		close(hpipe[1]);
		dup2(hpipe[0], STDIN_FILENO);
		close(hpipe[0]);
		*input_redirected = 1;
	}
}
