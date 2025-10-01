/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   create_redir_process.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 16:12:21 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/09/10 10:22:04 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

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

void	process_output_redir(t_cmd *cmd, int *i)
{
	if (!ft_strcmp(cmd->argv[*i], ">"))
		process_single_output_redir(cmd, i, REDIR_OUT);
	else if (!ft_strcmp(cmd->argv[*i], ">>"))
		process_single_output_redir(cmd, i, REDIR_APPEND);
}

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
