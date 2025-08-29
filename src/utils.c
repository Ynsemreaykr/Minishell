/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/29 00:03:35 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/08/29 00:19:44 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

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

void	cleanup_and_exit(int exit_code)
{
	ft_mem_cleanup();
	exit(exit_code);
}

void	free_cmds(t_cmd *cmd)
{
	t_cmd	*tmp;

	while (cmd)
	{
		tmp = cmd->next;
		free_args(cmd->argv);
		if (cmd->redirs)
			free_redir_list(cmd->redirs);
		if (cmd->heredocs)
			free_heredoc_list(cmd->heredocs);
		ft_free(cmd);
		cmd = tmp;
	}
}








void	update_last_arg(char **argv, t_shell *shell)
{
	int	i;

	if (!argv || !argv[0])
		return ;
	i = 0;
	while (argv[i])
		i++;
	if (i > 0)
	{
		if (shell->last_arg)
			ft_free(shell->last_arg);
		shell->last_arg = ft_strdup(argv[i - 1]);
	}
	else if (shell->last_arg)
	{
		ft_free(shell->last_arg);
		shell->last_arg = NULL;
	}
}

int	check_quotes(const char *input)
{
	int	in_single_quote;
	int	in_double_quote;
	int	i;

	in_single_quote = 0;
	in_double_quote = 0;
	i = 0;
	while (input[i])
	{
		if (input[i] == '\'' && !in_double_quote)
			in_single_quote = !in_single_quote;
		else if (input[i] == '"' && !in_single_quote)
			in_double_quote = !in_double_quote;
		i++;
	}
	if (in_single_quote)
		ft_putstr_fd("minishell: syntax error: unclosed single quote\n", 2);
	else if (in_double_quote)
		ft_putstr_fd("minishell: syntax error: unclosed double quote\n", 2);
	return (!(in_single_quote || in_double_quote));
}

static void	update_quote_flags(char c, int *in_quote, char *quote_char)
{
	if (*in_quote == 0 && (c == '"' || c == '\''))
	{
		*quote_char = c;
		*in_quote = 1;
	}
	else if (*in_quote && c == *quote_char)
	{
		*in_quote = 0;
		*quote_char = 0;
	}
}

static int	count_redir(const char *input, int *i, char redir_char)
{
	int	count;

	count = 0;
	while (input[*i] == redir_char)
	{
		count++;
		(*i)++;
	}
	return (count);
}

static int	validate_redir(char redir_char,
		int redir_count, const char *input, int i)
{
	int	j;

	if (redir_count > 2)
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
		j = 0;
		while (j < redir_count)
		{
			ft_putstr_fd(&redir_char, 2);
			j++;
		}
		ft_putstr_fd("'\n", 2);
		return (0);
	}
	if ((redir_count == 2 && redir_char == '<' && input[i] == '>')
		|| (redir_count == 2 && redir_char == '>' && input[i] == '<'))
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
		ft_putstr_fd("newline'\n", 2);
		return (0);
	}
	return (1);
}

static int	check_redir_operator(const char *input, int *i)
{
	char	redir_char;
	int		redir_count;

	redir_char = input[*i];
	redir_count = count_redir(input, i, redir_char);
	if (!validate_redir(redir_char, redir_count, input, *i))
		return (0);
	return (1);
}

int	check_redirection_syntax(const char *input)
{
	int		i;
	int		in_quote;
	char	quote_char;

	i = 0;
	in_quote = 0;
	quote_char = 0;
	while (input[i])
	{
		update_quote_flags(input[i], &in_quote, &quote_char);
		if (!in_quote && (input[i] == '>' || input[i] == '<'))
		{
			if (!check_redir_operator(input, &i))
				return (0);
		}
		else
			i++;
	}
	if (i > 0 && (input[i - 1] == '>' || input[i - 1] == '<'))
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
		ft_putstr_fd("newline'\n", 2);
		return (0);
	}
	return (1);
}

t_redir	*create_redir(t_redir_type type, char *filename)
{
	t_redir	*redir;

	redir = ft_malloc(sizeof(t_redir), __FILE__, __LINE__);
	if (!redir)
		return (NULL);
	redir->type = type;
	redir->filename = ft_strdup(filename);
	redir->next = NULL;
	return (redir);
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
		ft_free(current);
		current = next;
	}
}

t_heredoc	*create_heredoc(char *delimiter, char *cleaned_delimiter,
			int quoted_flag)
{
	t_heredoc	*heredoc;

	heredoc = ft_malloc(sizeof(t_heredoc), __FILE__, __LINE__);
	if (!heredoc)
		return (NULL);
	heredoc->delimiter = ft_strdup(delimiter);
	heredoc->cleaned_delimiter = ft_strdup(cleaned_delimiter);
	heredoc->quoted_flag = quoted_flag;
	heredoc->content = NULL;
	heredoc->next = NULL;
	return (heredoc);
}

void	add_heredoc(t_cmd *cmd, t_heredoc *heredoc)
{
	t_heredoc	*current;

	if (!cmd || !heredoc)
		return ;
	if (!cmd->heredocs)
		cmd->heredocs = heredoc;
	else
	{
		current = cmd->heredocs;
		while (current->next)
			current = current->next;
		current->next = heredoc;
	}
}

void	free_heredoc_list(t_heredoc *heredocs)
{
	t_heredoc	*current;
	t_heredoc	*next;

	current = heredocs;
	while (current)
	{
		next = current->next;
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

int	count_heredocs(t_heredoc *heredocs)
{
	int			count;
	t_heredoc	*current;

	count = 0;
	current = heredocs;
	while (current)
	{
		count++;
		current = current->next;
	}
	return (count);
}
