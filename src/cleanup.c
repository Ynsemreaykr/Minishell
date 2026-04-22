/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/29 00:03:35 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/10 10:21:44 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Komut ve shell yapılarının bellek temizleme modülü.
** free_args, free_cmds, free_redir_list ve cleanup_shell
** fonksiyonları burada tanımlanır. */

#include "../include/minishell.h"
#include <readline/readline.h>

/* Argüman dizisinin (char **) tüm elemanlarını ve diziyi serbest bırakır.
** args NULL ise hiçbir şey yapmaz. */
void	free_args(char **args)
{
	int	i;

	i = 0;
	if (!args)
		return ;
	while (args[i])
	{
		ft_free(args[i]);
		i++;
	}
	ft_free(args);
}

/* t_cmd bağlı listesinin tamamını serbest bırakır.
** Her düğüm için argv (free_args) ve redirs (free_redir_list) temizlenir,
** ardından düğümün kendisi serbest bırakılır. */
void	free_cmds(t_cmd *cmd)
{
	t_cmd	*tmp;

	while (cmd)
	{
		tmp = cmd->next;
		free_args(cmd->argv);
		if (cmd->redirs)
			free_redir_list(cmd->redirs);
		ft_free(cmd);
		cmd = tmp;
	}
}

/* t_redir bağlı listesinin tamamını serbest bırakır.
** Her düğüm için filename, delimiter, cleaned_delimiter ve content
** alanları ayrı ayrı kontrol edilerek serbest bırakılır. */
void	free_redir_list(t_redir *redirs)
{
	t_redir	*current;
	t_redir	*next;

	current = redirs;
	while (current)
	{
		next = current->next;
		if (current->filename)
			ft_free(current->filename);
		if (current->delimiter)
			ft_free(current->delimiter);
		if (current->cleaned_delimiter)
			ft_free(current->cleaned_delimiter);
		if (current->content)
			ft_free(current->content);
		ft_free(current);
		current = next;
	}
}

/* Shell kapanırken tüm kaynakları temizler:
** 1. cleanup_env → shell->env dizisini serbest bırakır
** 2. reset_signal_state → g_signal_number = 0
** 3. rl_clear_history → GNU readline geçmişini temizler
** 4. ft_mem_cleanup → tüm takip edilen belleği serbest bırakır */
void	cleanup_shell(t_shell *shell)
{
	cleanup_env(shell);
	reset_signal_state();
	rl_clear_history();
	ft_mem_cleanup();
}
