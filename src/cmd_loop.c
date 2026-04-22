/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_loop.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 01:55:00 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/10 10:21:49 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Komut listesi oluşturma döngüsü modülü.
** Her pipe segmenti için t_cmd düğümü oluşturur, tokenize eder,
** filtreler ve bağlı listeye ekler. */

#include "../include/minishell.h"

/* Argv dizisindeki boş stringleri ('') filtreler.
** Dahili komutlar için filtreleme yapılmaz (echo "" gibi durumlar korunur).
** Filtrelenmiş yeni bir dizi döndürülür, orijinal dizi serbest bırakılır. */
static char	**filter_empty_args(char **argv)
{
	int		i;
	int		j;
	int		count;
	char	**filtered;

	if (!argv)
		return (NULL);
	count = 0;
	i = 0;
	while (argv[i])
	{
		if (ft_strlen(argv[i]) > 0)
			count++;
		i++;
	}
	filtered = ft_malloc(sizeof(char *) * (count + 1));
	i = -1;
	j = 0;
	while (argv[++i])
	{
		if (ft_strlen(argv[i]) > 0)
			filtered[j++] = ft_strdup(argv[i]);
	}
	filtered[j] = NULL;
	return (filtered);
}

/* Yeni komut düğümünü bağlı listeye ekler.
** head boşsa hem head hem last güncellenir.
** Aksi hâlde son düğümün next'i ayarlanır ve last güncellenir. */
static void	add_cmd_to_list(t_cmd **head, t_cmd **last, t_cmd *cmd)
{
	if (!*head)
	{
		*head = cmd;
		*last = cmd;
	}
	else
	{
		(*last)->next = cmd;
		*last = cmd;
	}
}

/* Normal bir komut segmentini işler:
** 1. parse_redirections_and_heredoc → yönlendirmeleri t_redir listesine ayırır
** 2. Hem argv hem redirs boşsa 0 döner (boş komut, listeye ekleme)
** 3. Komutu listeye ekler, 1 döner. */
static int	process_regular_cmd(t_cmd_data *data, t_cmd **head, t_cmd **last)
{
	parse_redirections_and_heredoc(data->cmd);
	if (!data->cmd->argv && !data->cmd->redirs)
		return (0);
	data->cmd->next = NULL;
	add_cmd_to_list(head, last, data->cmd);
	return (1);
}

/* Tek bir komut segmenti için t_cmd oluşturur ve tokenize eder.
** split_tokens() ile giriş token'lara bölünür.
** Harici komutlar için boş argümanlar filter_empty_args ile temizlenir.
** Dahili komutlar direkt olarak (filtresiz) argv'ye atanır.
** Başarıda 1, hata durumunda 0 döner. */
static int	create_and_validate_cmd(t_cmd_data *data, int index)
{
	char	**original_argv;
	char	**filtered_argv;

	data->cmd = ft_malloc(sizeof(t_cmd));
	ft_memset(data->cmd, 0, sizeof(t_cmd));
	original_argv = split_tokens(
			data->cmd_strings[index], data->shell);
	if (!original_argv)
	{
		ft_putstr_fd(
			"minishell: syntax error near unexpected token\n", 2);
		return (0);
	}
	if (original_argv[0] && !is_builtin_command(original_argv[0]))
	{
		filtered_argv = filter_empty_args(original_argv);
		free_args(original_argv);
		data->cmd->argv = filtered_argv;
	}
	else
		data->cmd->argv = original_argv;
	return (1);
}

/* Tüm komut segmentleri için döngü:
** Her segment için create_and_validate_cmd → process_regular_cmd çağrılır.
** Hata durumunda cleanup_and_return_null ile temizlik yapılır ve 0 döner.
** argv[0] NULL olan komutlar (sadece redirs içerenler) direkt listeye eklenir.
** Başarıda 1 döner. */
int	process_cmd_loop(t_cmd_data *data, t_cmd **head, t_cmd **last)
{
	int	c;

	c = -1;
	while (++c < data->cmd_count)
	{
		if (!create_and_validate_cmd(data, c))
		{
			cleanup_and_return_null(
				data->cmd, data->cmd_strings, data->cmd_count);
			return (0);
		}
		if (data->cmd->argv && !data->cmd->argv[0])
		{
			add_cmd_to_list(head, last, data->cmd);
			continue ;
		}
		if (!process_regular_cmd(data, head, last))
		{
			cleanup_and_return_null(
				data->cmd, data->cmd_strings, data->cmd_count);
			return (0);
		}
	}
	return (1);
}
