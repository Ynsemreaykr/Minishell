/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   apply_redir_builtin.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/29 02:50:41 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/10 10:21:22 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Dahili komutlar için yönlendirme uygulama modülü.
** Fork yapılmadan çalıştırılan builtin komutlarında
** stdin/stdout geçici olarak yönlendirilir. */

#include "../include/minishell.h"
#include <unistd.h>
#include <stdio.h>

/* Heredoc yönlendirmesini dahili komut için uygular.
** r->content NULL ise 0 döner (içerik yok, işlem gerekmez).
** pipe() oluşturulur, içerik yazma ucuna yazılır, söküldür.
** dup2(hpipe[0], STDIN_FILENO) → dahili komut heredoc'tan okur.
** Hata durumunda -1, başarıda 0 döner. */
int	handle_heredoc_redir_builtin(t_redir *r, int *input_redirected)
{
	int	hpipe[2];

	if (!r->content)
		return (0);
	if (pipe(hpipe) != 0)
		return (-1);
	write(hpipe[1], r->content, ft_strlen(r->content));
	close(hpipe[1]);
	dup2(hpipe[0], STDIN_FILENO);
	close(hpipe[0]);
	*input_redirected = 1;
	return (0);
}

/* "< dosya" giriş yönlendirmesini dahili komut için uygular.
** Dosya açılamazsa perror + 1 döner.
** Başarıda dup2 ile stdin dosyaya bağlanır, 0 döner. */
int	redir_in_builtin(t_redir *r, int *input_redirected)
{
	int	infd;

	infd = open(r->filename, O_RDONLY);
	if (infd < 0)
	{
		perror(r->filename);
		return (1);
	}
	dup2(infd, STDIN_FILENO);
	close(infd);
	*input_redirected = 1;
	return (0);
}

/* "> dosya" çıkış yönlendirmesini dahili komut için uygular.
** O_TRUNC bayraklı open → dosya sıfırlanır.
** dup2 ile stdout dosyaya bağlanır.
** Hata durumunda perror + 1, başarıda 0 döner. */
int	redir_out_builtin(t_redir *r, int *output_redirected, int *outfd)
{
	*outfd = open(r->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (!*output_redirected)
		*output_redirected = 1;
	if (*outfd < 0)
	{
		perror(r->filename);
		return (1);
	}
	dup2(*outfd, STDOUT_FILENO);
	close(*outfd);
	return (0);
}

/* ">> dosya" ekleme yönlendirmesini dahili komut için uygular.
** O_APPEND bayraklı open → mevcut içerik korunur.
** dup2 ile stdout dosya sonuna yazar.
** Hata durumunda perror + 1, başarıda 0 döner. */
int	redir_append_builtin(t_redir *r, int *output_redirected, int *outfd)
{
	*outfd = open(r->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (!*output_redirected)
		*output_redirected = 1;
	if (*outfd < 0)
	{
		perror(r->filename);
		return (1);
	}
	dup2(*outfd, STDOUT_FILENO);
	close(*outfd);
	return (0);
}
