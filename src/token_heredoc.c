/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_heredoc.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 02:45:12 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/09/10 10:23:42 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

static char	*create_delimiter_token(const char *input, int start, int end)
{
	char	*delim;
	int		len;

	len = end - start;
	if (len <= 0)
		return (NULL);
	delim = ft_malloc(len + 1);
	ft_strncpy(delim, input + start, len);
	delim[len] = '\0';
	return (delim);
}

static void	handle_heredoc_quotes(const char *input, int *i,
								int *in_quote, char *quote_char)
{
	if ((input[*i] == '"' || input[*i] == '\'') && !*in_quote)
	{
		*quote_char = input[*i];
		*in_quote = 1;
		(*i)++;
	}
	else if (input[*i] == *quote_char && *in_quote)
	{
		*in_quote = 0;
		*quote_char = 0;
		(*i)++;
	}
	else
		(*i)++;
}

void	parse_heredoc_delimiter(const char *input, int *i,
								char **args, int *argc)
{
	int		start;
	int		in_quote;
	char	quote_char;
	char	*delim;

	*i = skip_whitespace(input, *i);
	start = *i;
	in_quote = 0;
	quote_char = 0;
	while (input[*i] && ((input[*i] != ' ' && input[*i] != '\t') || in_quote))
	{
		if (!in_quote && *i + 1 < (int)ft_strlen(input)
			&& input[*i] == '<' && input[*i + 1] == '<')
			break ;
		handle_heredoc_quotes(input, i, &in_quote, &quote_char);
	}
	delim = create_delimiter_token(input, start, *i);
	if (delim)
		args[(*argc)++] = delim;
}
