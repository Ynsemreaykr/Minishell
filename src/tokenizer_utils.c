/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/29 01:08:56 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/09/10 10:23:47 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

static void	process_unquoted_segment(const char *input, int *i)
{
	while (input[*i] && input[*i] != ' ' && input[*i] != '\t'
		&& input[*i] != '"' && input[*i] != '\''
		&& !check_redirection_operator(input, *i))
		(*i)++;
}

static void	process_quoted_segment(const char *input, int *i)
{
	char	quote_char;

	quote_char = input[*i];
	(*i)++;
	while (input[*i] && input[*i] != quote_char)
		(*i)++;
	if (input[*i] == quote_char)
		(*i)++;
}

static char	*build_concatenated_token(char *concatenated_token,
									char *segment, int *concat_len)
{
	int		segment_len;
	char	*new_token;

	if (segment)
	{
		segment_len = ft_strlen(segment);
		if (concatenated_token)
		{
			new_token = ft_malloc(*concat_len + segment_len + 1);
			ft_strcpy(new_token, concatenated_token);
			ft_strcat(new_token, segment);
			ft_free(concatenated_token);
			ft_free(segment);
			concatenated_token = new_token;
			*concat_len += segment_len;
		}
		else
		{
			concatenated_token = segment;
			*concat_len = segment_len;
		}
	}
	return (concatenated_token);
}

static void	handle_segment_parsing(const char *input, int *i)
{
	if ((input[*i] == '$' && (input[*i + 1] == '"' || input[*i + 1] == '\''))
		|| input[*i] == '"' || input[*i] == '\'')
	{
		if (input[*i] == '$')
			(*i)++;
		process_quoted_segment(input, i);
	}
	else
		process_unquoted_segment(input, i);
}

char	*process_regular_quoted_token(const char *input, int *i,
										t_shell *shell)
{
	char	*concatenated_token;
	char	*segment;
	int		segment_start;
	int		concat_len;

	concatenated_token = NULL;
	concat_len = 0;
	while (input[*i] && input[*i] != ' ' && input[*i] != '\t'
		&& !check_redirection_operator(input, *i))
	{
		segment_start = *i;
		handle_segment_parsing(input, i);
		if (*i > segment_start)
		{
			segment = process_token_content(input, segment_start, *i, shell);
			concatenated_token = build_concatenated_token(concatenated_token,
					segment, &concat_len);
		}
	}
	return (concatenated_token);
}
