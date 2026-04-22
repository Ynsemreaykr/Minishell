/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 16:02:53 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/10 10:21:41 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Ek temizlik yardımcıları.
** Heredoc içerik temizliği, env temizliği ve child process temizliği. */

#include "../include/minishell.h"

/* Tüm komut listesinde heredoc içeriklerini serbest bırakır.
** Her t_redir listesinde HEREDOC tipli olanların content alanı temizlenir.
** Pipeline tamamlandıktan sonra çağrılır (içerik artık kullanılmaz). */
void	cleanup_heredoc_content(t_cmd *cmds)
{
	t_cmd	*current;
	t_redir	*redir;

	current = cmds;
	while (current)
	{
		if (current->redirs)
		{
			redir = current->redirs;
			while (redir)
			{
				if (redir->type == HEREDOC && redir->content)
				{
					ft_free(redir->content);
					redir->content = NULL;
				}
				redir = redir->next;
			}
		}
		current = current->next;
	}
}

/* shell->env dizisinin tüm elemanlarını ve diziyi serbest bırakır.
** shell NULL ise hemen döner. Temizleme sonrası shell->env = NULL yapılır. */
void	cleanup_env(t_shell *shell)
{
	int	i;

	if (!shell)
		return ;
	if (shell->env)
	{
		i = 0;
		while (shell->env[i])
		{
			ft_free(shell->env[i]);
			i++;
		}
		ft_free(shell->env);
		shell->env = NULL;
	}
}

/* Child process sonlanmadan önce shell kaynaklarını temizler.
** cleanup_env ile ortam değişkenleri, ft_mem_cleanup ile
** tüm takip edilen bellek serbest bırakılır.
** execve öncesi veya child'ın exit() çağrısından önce kullanılır. */
void	cleanup_shell_for_child(t_shell *shell)
{
	if (!shell)
		return ;
	cleanup_env(shell);
	ft_mem_cleanup();
}

/* Kısmi olarak oluşturulmuş bir t_cmd düğümünü serbest bırakır.
** argv dizisi ve redir listesi ayrı ayrı temizlendikten sonra
** düğümün kendisi ft_free ile kaldırılır. */
static void	free_cmd_struct(t_cmd *cmd)
{
	int	i;

	if (!cmd)
		return ;
	if (cmd->argv)
	{
		i = 0;
		while (cmd->argv[i])
			ft_free(cmd->argv[i++]);
		ft_free(cmd->argv);
	}
	if (cmd->redirs)
		free_redir_list(cmd->redirs);
	ft_free(cmd);
}

/* Hata durumunda kısmen oluşturulmuş yapıları temizler.
** cmd NULL değilse free_cmd_struct ile temizlenir.
** cmd_strings NULL değilse cmd_count kadar eleman serbest bırakılır. */
void	cleanup_and_return_null(t_cmd *cmd, char **cmd_strings, int cmd_count)
{
	int	i;

	if (cmd)
		free_cmd_struct(cmd);
	if (cmd_strings)
	{
		i = 0;
		while (i < cmd_count)
			ft_free(cmd_strings[i++]);
		ft_free(cmd_strings);
	}
}
