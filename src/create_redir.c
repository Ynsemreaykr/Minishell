/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   create_redir.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 16:12:21 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/09/10 10:22:06 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

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
