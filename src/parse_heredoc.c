/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_heredoc.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 17:36:54 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/09/10 10:23:13 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

static void	process_input_redir(t_cmd *cmd, int *i, char **new_argv, int *j)
{
	int			quote_type;
	char		*original;
	t_redir		*heredoc_redir;

	if (!ft_strcmp(cmd->argv[*i], "<"))
		handle_in_redir(cmd, cmd->argv, i);
	else if (!ft_strcmp(cmd->argv[*i], "<<"))
	{
		if (is_valid_redir_target(cmd->argv[*i + 1]))
		{
			original = ft_strdup(cmd->argv[*i + 1]);
			quote_type = detect_quote_clean_delimiter(&cmd->argv[*i + 1]);
			heredoc_redir = create_heredoc_redir(original,
					cmd->argv[*i + 1], quote_type);
			if (heredoc_redir)
				add_redir(cmd, heredoc_redir);
			ft_free(original);
			*i += 2;
		}
		else
			*i += 1;
	}
	else
		new_argv[(*j)++] = ft_strdup(cmd->argv[(*i)++]);
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
		{
			if (is_valid_redir_target(argv[i + 1]))
				i += 2;
			else
				i++;
		}
		else
		{
			argc++;
			i++;
		}
	}
	return (argc);
}

void	parse_redirections_and_heredoc(t_cmd *cmd)
{
	int		argc;
	char	**new_argv;

	argc = count_non_redir_args(cmd->argv);
	new_argv = ft_malloc(sizeof(char *) * (argc + 1));
	process_argv_elements(cmd, new_argv);
	cleanup_and_replace_argv(cmd, new_argv, argc);
}
