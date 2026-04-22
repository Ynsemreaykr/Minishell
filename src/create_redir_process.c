/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   create_redir_process.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 16:12:21 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/10 10:22:04 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Çıkış (>) ve giriş (<) yönlendirme işleme fonksiyonları.
** Token dizisinden yönlendirme düğümleri oluşturulur ve cmd'ye eklenir. */

#include "../include/minishell.h"

/* Tek bir çıkış yönlendirme kaydı oluşturur ve listeye ekler.
** - Hedef token geçerliyse (is_valid_redir_target) create_redir çağrılır
** - i iki ilerletilir (operatör + hedef)
** - Geçersiz hedefte yalnızca i bir ilerletilir */
static void	process_single_output_redir(t_cmd *cmd, int *i, t_redir_type type)
{
	t_redir	*redir;

	if (is_valid_redir_target(cmd->argv[*i + 1]))
	{
		redir = create_redir(type, cmd->argv[*i + 1]);
		if (redir)
			add_redir(cmd, redir);
		*i += 2;
	}
	else
		*i += 1;
}

/* ">" → REDIR_OUT, ">>" → REDIR_APPEND işleme üst fonksiyonu.
** process_single_output_redir'e uygun tip ile iletilir. */
void	process_output_redir(t_cmd *cmd, int *i)
{
	if (!ft_strcmp(cmd->argv[*i], ">"))
		process_single_output_redir(cmd, i, REDIR_OUT);
	else if (!ft_strcmp(cmd->argv[*i], ">>"))
		process_single_output_redir(cmd, i, REDIR_APPEND);
}

/* "<" operatörü için REDIR_IN düğümü oluşturur ve cmd'ye ekler.
** - Hedef token geçerliyse create_redir + add_redir
** - i ikişer ilerletilir (operatör + hedef)
** - Geçersiz hedefte yalnızca bir ilerletilir */
void	handle_in_redir(t_cmd *cmd, char **argv, int *i)
{
	t_redir	*redir;

	if (is_valid_redir_target(argv[*i + 1]))
	{
		redir = create_redir(REDIR_IN, argv[*i + 1]);
		if (redir)
			add_redir(cmd, redir);
		*i += 2;
	}
	else
		*i += 1;
}
