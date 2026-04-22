/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_handlers.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 08:39:38 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/10 10:21:54 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Komut yürütme (execution) işleminin ilk giriş ve dağıtım (router) yüzü.
** Öncelikle komutlara ait tüm heredoc verilerini okur,
** ardından komutun tek mi yoksa pipe lı mı olduğuna karar verir. */

#include "../include/minishell.h"

/* Bir komut (t_cmd) nesnesinin içindeki yönlendirmeleri gözden geçirir
** ve içlerinde HEREDOC tespit ederse process_single_heredoc fonksiyonunu çağırır.
** Heredoc çalışırken hata (örn. Ctrl+C) oluşursa -1 döner, içeriği temizler
** ve kabuğa dönüş değerini (exit_code) işler. */
static int	handle_cmd_heredocs(t_cmd *cmd, t_shell *shell, t_cmd *all_cmds)
{
	t_redir	*redir;
	int		result;

	if (!cmd->redirs)
		return (0);
	redir = cmd->redirs;
	while (redir)
	{
		if (redir->type == HEREDOC)
		{
			result = process_single_heredoc(redir, shell);
			if (result != 0)
			{
				cleanup_heredoc_content(all_cmds);
				shell->last_exit = result;
				return (-1);
			}
		}
		redir = redir->next;
	}
	return (0);
}

/* Pipe ile bağlı olabilen tüm komut dizilerini (cmds listesini) tek tek dolaşır.
** Komutlarda heredoc olup olmadığını denetler. Command execute edilmeden
** evvel tüm heredoc içeriklerinin stdin'e bağlanmak üzere okunmasını sağlar. */
static int	handle_all_heredocs(t_cmd *cmds, t_shell *shell)
{
	t_cmd	*current;
	int		result;

	current = cmds;
	while (current)
	{
		result = handle_cmd_heredocs(current, shell, cmds);
		if (result != 0)
			return (-1);
		current = current->next;
	}
	return (1);
}

/* Pipe olmayan, yani çalıştırılacak sadece 1 adet komut varsa bu fonksiyon yürütülür.
** Komut is_builtin ile kontrol edilir, dahilise execute_single_builtin çalışır (forksuz).
** Değilse execute_single_external ile fork yapılarak harici bir program olarak yürütülür.
** Çıkış kodu ($? yani last_exit) setlenip başarılı olunursa 1, olmazsa 0 döner. */
static int	execute_single_command(t_cmd *cmd, t_shell *shell)
{
	int	result;

	if (cmd->argv && cmd->argv[0] && is_builtin(cmd->argv[0]))
	{
		result = execute_single_builtin(cmd, shell);
		shell->last_exit = result;
		if (result == 0)
			return (1);
		return (0);
	}
	result = execute_single_external(cmd, shell);
	shell->last_exit = result;
	if (result == 0)
		return (1);
	return (0);
}

/* Program komut dizilerini parserdan alıp çalıştırmaya burayla başlar!
** 1. Tüm heredoc direktifleri kullanıcıdan okunur.
** 2. Eğer birden fazla komut pipe ile (cmds->next) bağlıysa pipeline exec fonksiyonuyla çalışır.
** 3. Tek bir komut komut varsa (cmds->next yoksa) single_command olarak çalışır.
** Genel başarı değerini döndürür. */
int	execute_command_main(t_cmd *cmds, t_shell *shell)
{
	int	result;

	if (handle_all_heredocs(cmds, shell) == -1)
		return (-1);
	if (cmds->next)
	{
		result = exec_pipeline(cmds, shell);
		shell->last_exit = result;
		if (result == 0)
			return (1);
		return (0);
	}
	return (execute_single_command(cmds, shell));
}
