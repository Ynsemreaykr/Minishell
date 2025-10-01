/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   count_token_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 16:12:21 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/09/10 10:22:01 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

int	count_quoted_token(const char *input, int *i)
{
	char	quote_char;

	quote_char = input[*i];
	(*i)++;
	while (input[*i] && input[*i] != quote_char)
		(*i)++;
	if (input[*i] == quote_char)
		(*i)++;
	return (1);
}

int	count_redirection_token(const char *input, int *i)
{
	int	token_count;
	int	redir_len;

	token_count = 0;
	redir_len = get_redirection_length(input, *i);
	if (redir_len > 0)
	{
		token_count++;
		*i += redir_len;
	}
	return (token_count);
}

int	process_regular_token(const char *input, int *pos,
									int *in_quote, char *quote_char)
{
	int	token_count;
	int	start_pos;

	token_count = 0;
	start_pos = *pos;
	while (input[*pos] && ((input[*pos] != ' ' && input[*pos] != '\t')
			|| *in_quote))
	{
		if (!*in_quote && check_redirection_operator(input, *pos))
			break ;
		update_quote_state(input, pos, in_quote, quote_char);
	}
	if (*pos > start_pos)
		token_count = 1;
	return (token_count);
}
