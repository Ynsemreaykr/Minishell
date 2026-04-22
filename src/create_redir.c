/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   create_redir.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 16:12:21 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/10 10:22:06 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* t_redir düğümü oluşturma ve bağlı listeye ekleme modülü. */

#include "../include/minishell.h"

/* Verilen token'ın geçerli bir yönlendirme hedefi olup olmadığını kontrol eder.
** |, >, >>, <, << ise geçersizdir (yönlendirme operatörü) → 0 döner.
** NULL ise → 0, normal string ise → 1 döner. */
int	is_valid_redir_target(const char *token)
{
	if (!token)
		return (0);
	if (!ft_strcmp(token, "|")
		|| !ft_strcmp(token, ">")
		|| !ft_strcmp(token, ">>")
		|| !ft_strcmp(token, "<")
		|| !ft_strcmp(token, "<<"))
		return (0);
	return (1);
}

/* Komutun yönlendirme listesinin sonuna yeni bir t_redir düğümü ekler.
** Liste boşsa direkt olarak cmd->redirs yapılır.
** Aksi hâlde son düğüme kadar ilerlenip oraya eklenir. */
void	add_redir(t_cmd *cmd, t_redir *redir)
{
	t_redir	*current;

	if (!cmd || !redir)
		return ;
	if (!cmd->redirs)
		cmd->redirs = redir;
	else
	{
		current = cmd->redirs;
		while (current->next)
			current = current->next;
		current->next = redir;
	}
}

/* REDIR_IN, REDIR_OUT veya REDIR_APPEND tipi için t_redir düğümü oluşturur.
** filename alanı ft_strdup ile kopyalanır.
** delimiter, cleaned_delimiter, content → NULL; quoted_flag → 0 */
t_redir	*create_redir(t_redir_type type, char *filename)
{
	t_redir	*redir;

	redir = ft_malloc(sizeof(t_redir));
	if (!redir)
		return (NULL);
	redir->type = type;
	redir->filename = ft_strdup(filename);
	redir->delimiter = NULL;
	redir->cleaned_delimiter = NULL;
	redir->content = NULL;
	redir->quoted_flag = 0;
	redir->next = NULL;
	return (redir);
}

/* HEREDOC tipi için t_redir düğümü oluşturur.
** - delimiter: ham (tırnaklı) delimiter string kopyası
** - cleaned_delimiter: tırnakları temizlenmiş versiyon
** - quoted_flag: 1 = tek tırnak, 2 = çift tırnak, 0 = tırnaksız
** filename → NULL; content → NULL (heredoc okuma sonrası doldurulur) */
t_redir	*create_heredoc_redir(char *delimiter,
		char *cleaned_delimiter, int quoted_flag)
{
	t_redir	*redir;

	redir = ft_malloc(sizeof(t_redir));
	if (!redir)
		return (NULL);
	redir->type = HEREDOC;
	redir->filename = NULL;
	redir->delimiter = ft_strdup(delimiter);
	redir->cleaned_delimiter = ft_strdup(cleaned_delimiter);
	redir->content = NULL;
	redir->quoted_flag = quoted_flag;
	redir->next = NULL;
	return (redir);
}
