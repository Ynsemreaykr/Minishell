/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/29 01:08:56 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/08/29 02:34:56 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include <stdlib.h>
#include <stdio.h>

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

static void	process_unquoted_segment(const char *input, int *i)
{
	while (input[*i] && input[*i] != ' ' && input[*i] != '\t'
		&& input[*i] != '"' && input[*i] != '\''
		&& !check_redirection_operator(input, *i))
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
			new_token = ft_malloc(*concat_len + segment_len + 1,
					__FILE__, __LINE__);
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

static char	*create_delimiter_token(const char *input, int start, int end)
{
	char	*delim;
	int		len;

	len = end - start;
	if (len <= 0)
		return (NULL);
	delim = ft_malloc(len + 1, __FILE__, __LINE__);
	ft_strncpy(delim, input + start, len);
	delim[len] = '\0';
	return (delim);
}

static char	**initialize_token_array(int token_count, int *should_return)
{
	char	**args;

	*should_return = 0;
	if (token_count == -1)
	{
		*should_return = 1;
		return (NULL);
	}
	if (token_count <= 0)
	{
		args = ft_malloc(sizeof(char *), __FILE__, __LINE__);
		args[0] = NULL;
		*should_return = 1;
		return (args);
	}
	args = ft_malloc(sizeof(char *) * (token_count + 1), __FILE__, __LINE__);
	return (args);
}

static char	*process_regular_quoted_token(const char *input, int *i,
										int last_exit, t_shell *shell)
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
		if (input[*i] == '"' || input[*i] == '\'')
			process_quoted_segment(input, i);
		else
			process_unquoted_segment(input, i);
		if (*i > segment_start)
		{
			segment = process_token_content(input, segment_start, *i,
					last_exit, shell);
			concatenated_token = build_concatenated_token(concatenated_token,
					segment, &concat_len);
		}
	}
	return (concatenated_token);
}

static void	add_token_to_args(char *token, char **args, int *argc)
{
	if (token && ft_strlen(token) > 0)
		args[(*argc)++] = token;
	else if (token)
		ft_free(token);
}

static void	parse_heredoc_delimiter(const char *input, int *i,
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

static void	handle_redirection_token(const char *input, int *i,
								char **args, int *argc)
{
	int		redir_len;
	char	*redir_token;

	redir_len = get_redirection_length(input, *i);
	if (redir_len > 0)
	{
		redir_token = ft_malloc(redir_len + 1, __FILE__, __LINE__);
		ft_strncpy(redir_token, input + *i, redir_len);
		redir_token[redir_len] = '\0';
		args[(*argc)++] = redir_token;
		*i += redir_len;
		if (redir_len == 2 && input[*i - 2] == '<' && input[*i - 1] == '<')
			parse_heredoc_delimiter(input, i, args, argc);
	}
}

char	**split_tokens(const char *input, int last_exit, t_shell *shell)
{
	char	**args;
	char	*token;
	int		argc;
	int		i;
	int		should_return;

	args = initialize_token_array(count_tokens(input), &should_return);
	if (should_return)
		return (args);
	argc = 0;
	i = 0;
	while (input[i])
	{
		i = skip_whitespace(input, i);
		if (!input[i])
			break ;
		if (!check_redirection_operator(input, i))
		{
			token = process_regular_quoted_token(input, &i, last_exit, shell);
			add_token_to_args(token, args, &argc);
		}
		else
			handle_redirection_token(input, &i, args, &argc);
	}
	return (args[argc] = NULL, args);
}

static int	process_regular_token(const char *input, int *pos,
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

static int	count_quoted_token(const char *input, int *i)
{
	int		token_count;
	char	quote_char;

	token_count = 0;
	quote_char = input[*i];
	(*i)++;
	token_count++;
	while (input[*i] && input[*i] != quote_char)
		(*i)++;
	if (input[*i] == quote_char)
		(*i)++;
	return (token_count);
}

static int	count_redirection_token(const char *input, int *i)
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

int	count_tokens(const char *input)
{
	int		i;
	int		token_count;
	int		in_quote;
	char	quote_char;

	i = 0;
	token_count = 0;
	in_quote = 0;
	quote_char = 0;
	while (input[i])
	{
		i = skip_whitespace(input, i);
		if (!input[i])
			break ;
		if ((input[i] == '"' || input[i] == '\'') && !in_quote)
			token_count += count_quoted_token(input, &i);
		else if (!in_quote && check_redirection_operator(input, i))
			token_count += count_redirection_token(input, &i);
		else if (process_regular_token(input, &i, &in_quote, &quote_char) > 0)
			token_count++;
	}
	return (token_count);
}
