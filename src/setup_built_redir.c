/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup_built_redir.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/29 02:50:41 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/10 10:23:32 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Dahili komutlar için yönlendirme kurulumu üst modülü.
** Her t_redir düğümü türüne göre uygun builtin yönlendirme fonksiyonuna iletilir. */

#include "../include/minishell.h"

/* Tek bir yönlendirme kaydını dahili komut için uygular.
** Tür kontrolüne göre ilgili builtin yönlendirme fonksiyonu çağrılır:
** - REDIR_IN  → redir_in_builtin
** - REDIR_OUT → redir_out_builtin
** - REDIR_APPEND → redir_append_builtin
** - HEREDOC   → handle_heredoc_redir_builtin
** Herhangi bir hata durumunda -1 döndürülür. */
static int	apply_single_redir_builtin(t_redir *r, int *input_redirected,
				int *output_redirected, int *outfd)
{
	if (r->type == REDIR_IN)
	{
		if (redir_in_builtin(r, input_redirected) == 1)
			return (-1);
	}
	else if (r->type == REDIR_OUT)
	{
		if (redir_out_builtin(r, output_redirected, outfd) == 1)
			return (-1);
	}
	else if (r->type == REDIR_APPEND)
	{
		if (redir_append_builtin(r, output_redirected, outfd) == 1)
			return (-1);
	}
	else if (r->type == HEREDOC)
	{
		if (handle_heredoc_redir_builtin(r, input_redirected) == -1)
			return (-1);
	}
	return (0);
}

/* Komutun tüm yönlendirme listesini dahili komut için uygular.
** Her t_redir kaydı için apply_single_redir_builtin çağrılır.
** Herhangi bir hata durumunda -1 döndürülür, başarıda 0. */
int	setup_redirections_for_builtin(t_cmd *cmd)
{
	int		input_redirected;
	int		output_redirected;
	int		outfd;
	t_redir	*r;

	input_redirected = 0;
	output_redirected = 0;
	outfd = -1;
	if (cmd->redirs)
	{
		r = cmd->redirs;
		while (r)
		{
			if (apply_single_redir_builtin(r, &input_redirected,
					&output_redirected, &outfd) == -1)
				return (-1);
			r = r->next;
		}
	}
	return (0);
}
