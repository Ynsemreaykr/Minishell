/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 17:36:54 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/08/28 20:21:24 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

typedef struct s_cmd_data
{
	t_cmd	*cmd;
	char	**cmd_strings;
	int		cmd_count;
	int		last_exit_code;
	t_shell	*shell;
}	t_cmd_data;

static int	is_in_quotes(const char *input, int pos)
{
	int		i;
	int		in_quote;
	char	quote_char;

	i = 0;
	in_quote = 0;
	quote_char = 0;
	while (i <= pos)
	{
		if ((input[i] == '"' || input[i] == '\'') && !in_quote)
		{
			quote_char = input[i];
			in_quote = 1;
		}
		else if (input[i] == quote_char && in_quote)
		{
			in_quote = 0;
			quote_char = 0;
		}
		i++;
	}
	return (in_quote);
}

static int	pipe_check(const char *input)
{
	int		len;
	int		i;
	int		j;

	len = ft_strlen(input);
	i = 0;
	while (i < len && (input[i] == ' ' || input[i] == '\t'))
		i++;
	if (i < len && input[i] == '|')
		return (0);
	if (len > 0 && input[len - 1] == '|')
		return (0);
	i = -1;
	while (i++ < len - 1)
	{
		if (input[i] == '|' && !is_in_quotes(input, i))
		{
			j = i + 1;
			while (j < len && (input[j] == ' ' || input[j] == '\t'))
				j++;
			if (j < len && input[j] == '|')
				return (0);
		}
	}
	return (1);
}

static void	clean_quotes_from_delimiter(char **delimiter, int len)
{
	char	*delim;
	char	*cleaned;

	delim = *delimiter;
	cleaned = ft_malloc(len - 1, __FILE__, __LINE__);
	ft_strncpy(cleaned, delim + 1, len - 2);
	cleaned[len - 2] = '\0';
	ft_free(*delimiter);
	*delimiter = cleaned;
}

static int	detect_quote_clean_delimiter(char **delimiter)
{
	char	*delim;
	int		len;

	if (!*delimiter || !**delimiter)
		return (0);
	delim = *delimiter;
	len = ft_strlen(delim);
	if (len >= 2 && delim[0] == '\'' && delim[len - 1] == '\'')
	{
		clean_quotes_from_delimiter(delimiter, len);
		return (1);
	}
	if (len >= 2 && delim[0] == '"' && delim[len - 1] == '"')
	{
		clean_quotes_from_delimiter(delimiter, len);
		return (2);
	}
	return (0);
}

static void	process_output_redir(t_cmd *cmd, int *i)
{
	t_redir	*redir;

	if (!ft_strcmp(cmd->argv[*i], ">"))
	{
		if (cmd->argv[*i + 1])
		{
			redir = create_redir(REDIR_OUT, cmd->argv[*i + 1]);
			if (redir)
				add_redir(cmd, redir);
		}
		*i += 2;
	}
	else if (!ft_strcmp(cmd->argv[*i], ">>"))
	{
		if (cmd->argv[*i + 1])
		{
			redir = create_redir(REDIR_APPEND, cmd->argv[*i + 1]);
			if (redir)
				add_redir(cmd, redir);
		}
		*i += 2;
	}
}

static void	handle_in_redir(t_cmd *cmd, char **argv, int *i)
{
	t_redir	*redir;

	if (argv[*i + 1])
	{
		redir = create_redir(REDIR_IN, argv[*i + 1]);
		if (redir)
			add_redir(cmd, redir);
		*i += 2;
	}
}

static void	process_input_redir(t_cmd *cmd, int *i, char **new_argv, int *j)
{
	int			quote_type;
	char		*original;
	t_heredoc	*heredoc;

	if (!ft_strcmp(cmd->argv[*i], "<"))
		handle_in_redir(cmd, cmd->argv, i);
	else if (!ft_strcmp(cmd->argv[*i], "<<"))
	{
		if (cmd->argv[*i + 1])
		{
			original = ft_strdup(cmd->argv[*i + 1]);
			quote_type = detect_quote_clean_delimiter(&cmd->argv[*i + 1]);
			heredoc = create_heredoc(original, cmd->argv[*i + 1], quote_type);
			if (heredoc)
				add_heredoc(cmd, heredoc);
			ft_free(original);
		}
		*i += 2;
	}
	else
		new_argv[(*j)++] = ft_strdup(cmd->argv[(*i)++]);
}

static int	count_non_redir_args(char **argv)
{
	int	i;
	int	argc;

	i = 0;
	argc = 0;
	while (argv && argv[i])
	{
		if (!ft_strcmp(argv[i], ">") || !ft_strcmp(argv[i], ">>")
			|| !ft_strcmp(argv[i], "<") || !ft_strcmp(argv[i], "<<"))
			i += 2;
		else
		{
			argc++;
			i++;
		}
	}
	return (argc);
}

static void	process_argv_elements(t_cmd *cmd, char **new_argv)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (cmd->argv && cmd->argv[i])
	{
		if (!ft_strcmp(cmd->argv[i], ">") || !ft_strcmp(cmd->argv[i], ">>"))
			process_output_redir(cmd, &i);
		else
			process_input_redir(cmd, &i, new_argv, &j);
	}
	new_argv[j] = NULL;
}

static void	cleanup_and_replace_argv(t_cmd *cmd, char **new_argv, int argc)
{
	int	k;

	k = 0;
	while (cmd->argv && cmd->argv[k])
		ft_free(cmd->argv[k++]);
	ft_free(cmd->argv);
	if (argc == 0)
	{
		ft_free(new_argv);
		cmd->argv = NULL;
	}
	else
		cmd->argv = new_argv;
}

static void	parse_redirections_and_heredoc(t_cmd *cmd)
{
	int		argc;
	char	**new_argv;

	argc = count_non_redir_args(cmd->argv);
	new_argv = ft_malloc(sizeof(char *) * (argc + 1), __FILE__, __LINE__);
	process_argv_elements(cmd, new_argv);
	cleanup_and_replace_argv(cmd, new_argv, argc);
}

static int	validate_input_and_pipes(const char *input)
{
	if (!pipe_check(input))
	{
		ft_putstr_fd(
			"minishell: syntax error near unexpected token `|'\n", 2);
		return (0);
	}
	return (1);
}

static char	**get_cmd_strings(const char *input, int *cmd_count)
{
	char	**cmd_strings;

	cmd_strings = split_by_pipes(input, cmd_count);
	if (!cmd_strings)
	{
		ft_putstr_fd(
			"minishell: syntax error near unexpected token `|'\n", 2);
		return (NULL);
	}
	return (cmd_strings);
}

static int	create_and_validate_cmd(t_cmd_data *data, int index)
{
	data->cmd = ft_malloc(sizeof(t_cmd), __FILE__, __LINE__);
	ft_memset(data->cmd, 0, sizeof(t_cmd));
	data->cmd->argv = split_tokens(data->cmd_strings[index],
			data->last_exit_code, data->shell);
	if (!data->cmd->argv && !data->cmd->heredocs)
	{
		ft_putstr_fd(
			"minishell: syntax error near unexpected token\n", 2);
		return (0);
	}
	return (1);
}

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

static int	validate_redirection_syntax(t_cmd *cmd)
{
	int	i;

	if (!cmd->argv)
		return (1);
	i = -1;
	while (cmd->argv[++i])
	{
		if ((!ft_strcmp(cmd->argv[i], ">") || !ft_strcmp(cmd->argv[i], ">>")
				|| !ft_strcmp(cmd->argv[i], "<")
				|| !ft_strcmp(cmd->argv[i], "<<")) && i > 0)
		{
			if (!ft_strcmp(cmd->argv[i - 1], ">")
				|| !ft_strcmp(cmd->argv[i - 1], ">>")
				|| !ft_strcmp(cmd->argv[i - 1], "<")
				|| !ft_strcmp(cmd->argv[i - 1], "<<"))
			{
				ft_putstr_fd(
					"minishell: syntax error near unexpected token `>'\n", 2);
				ft_putstr_fd(cmd->argv[i], 2);
				ft_putstr_fd("'\n", 2);
				return (0);
			}
		}
	}
	return (1);
}

static int	process_regular_cmd(t_cmd_data *data, t_cmd **head, t_cmd **last)
{
	if (!validate_redirection_syntax(data->cmd))
		return (0);
	parse_redirections_and_heredoc(data->cmd);
	if (!data->cmd->argv && !data->cmd->heredocs && !data->cmd->redirs)
		return (0);
	data->cmd->next = NULL;
	add_cmd_to_list(head, last, data->cmd);
	return (1);
}

static int	process_cmd_loop(t_cmd_data *data, t_cmd **head, t_cmd **last)
{
	int	c;

	c = -1;
	while (++c < data->cmd_count)
	{
		if (!create_and_validate_cmd(data, c))
		{
			cleanup_and_return_null(data->cmd, data->cmd_strings,
				data->cmd_count);
			return (0);
		}
		if (data->cmd->argv && !data->cmd->argv[0])
		{
			add_cmd_to_list(head, last, data->cmd);
			c++;
			continue ;
		}
		if (!process_regular_cmd(data, head, last))
		{
			cleanup_and_return_null(data->cmd, data->cmd_strings,
				data->cmd_count);
			return (0);
		}
	}
	return (1);
}

static void	cleanup_cmd_strings(char **cmd_strings, int cmd_count)
{
	int	c;

	c = 0;
	while (c < cmd_count)
		ft_free(cmd_strings[c++]);
	ft_free(cmd_strings);
}

t_cmd	*parse_commands(const char *input, int last_exit, t_shell *shell)
{
	t_cmd		*head;
	t_cmd		*last;
	t_cmd_data	data;
	int			cmd_count;
	char		**cmd_strings;

	if (!validate_input_and_pipes(input))
		return (NULL);
	head = NULL;
	last = NULL;
	cmd_strings = get_cmd_strings(input, &cmd_count);
	if (!cmd_strings)
		return (NULL);
	data.cmd_strings = cmd_strings;
	data.cmd_count = cmd_count;
	data.last_exit_code = last_exit;
	data.shell = shell;
	if (!process_cmd_loop(&data, &head, &last))
		return (NULL);
	cleanup_cmd_strings(cmd_strings, cmd_count);
	return (head);
}
