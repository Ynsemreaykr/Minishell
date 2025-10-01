/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/29 01:08:56 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/09/10 10:23:50 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

static char	**initialize_token_array(int token_count, int *should_return)
{
	char	**args;

	*should_return = 0;
	if (token_count <= 0)
	{
		args = ft_malloc(sizeof(char *));
		args[0] = NULL;
		*should_return = 1;
		return (args);
	}
	args = ft_malloc(sizeof(char *) * (token_count + 1));
	return (args);
}

static void	handle_redirection_token(const char *input, int *i,
								char **args, int *argc)
{
	int		redir_len;
	char	*redir_token;

	redir_len = get_redirection_length(input, *i);
	if (redir_len > 0)
	{
		redir_token = ft_malloc(redir_len + 1);
		ft_strncpy(redir_token, input + *i, redir_len);
		redir_token[redir_len] = '\0';
		args[(*argc)++] = redir_token;
		*i += redir_len;
		if (redir_len == 2 && input[*i - 2] == '<' && input[*i - 1] == '<')
			parse_heredoc_delimiter(input, i, args, argc);
	}
}

static void	process_token_loop(const char *input, t_shell *shell,
			char **args, int *argc)
{
	int		i;
	char	*token;

	i = 0;
	while (input[i])
	{
		i = skip_whitespace(input, i);
		if (!input[i])
			break ;
		if (!check_redirection_operator(input, i))
		{
			token = process_regular_quoted_token(input, &i, shell);
			if (token)
				args[(*argc)++] = token;
		}
		else
			handle_redirection_token(input, &i, args, argc);
	}
}

char	**split_tokens(const char *input, t_shell *shell)
{
	char	**args;
	int		argc;
	int		should_return;

	args = initialize_token_array(count_tokens(input), &should_return);
	if (should_return)
		return (args);
	argc = 0;
	process_token_loop(input, shell, args, &argc);
	args[argc] = NULL;
	return (args);
}

char	*process_single_quote(t_proc_ctx *ctx)
{
	(*(ctx->i))++;
	while (*(ctx->i) < ctx->end && ctx->input[*(ctx->i)] != '\'')
	{
		ctx->processed[*(ctx->proc_len)] = ctx->input[*(ctx->i)];
		(*(ctx->proc_len))++;
		(*(ctx->i))++;
	}
	if (*(ctx->i) < ctx->end && ctx->input[*(ctx->i)] == '\'')
		(*(ctx->i))++;
	return (ctx->processed);
}
