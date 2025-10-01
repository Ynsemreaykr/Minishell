/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_processing.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 01:55:00 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/09/10 10:23:44 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

static char	*process_double_quote(t_proc_ctx *ctx)
{
	(*(ctx->i))++;
	while (*(ctx->i) < ctx->end && ctx->input[*(ctx->i)] != '"')
	{
		if (ctx->input[*(ctx->i)] == '$')
		{
			if (*(ctx->i) + 1 < ctx->end && ctx->input[*(ctx->i) + 1] == '"')
				process_literal_dollar(ctx->processed, ctx->proc_len, ctx->i);
			else if (*(ctx->i) + 1 < ctx->end
				&& ctx->input[*(ctx->i) + 1] == '?')
				process_exit_status(ctx->shell->last_exit,
					ctx->processed, ctx->proc_len, ctx->i);
			else if (*(ctx->i) + 1 < ctx->end
				&& (ft_isalnum(ctx->input[*(ctx->i) + 1])
					|| ctx->input[*(ctx->i) + 1] == '_'))
				process_variable_expansion_in_quotes(ctx);
			else
				process_literal_dollar(ctx->processed, ctx->proc_len, ctx->i);
		}
		else
			process_regular_char(ctx->input, ctx->i,
				ctx->processed, ctx->proc_len);
	}
	if (*(ctx->i) < ctx->end && ctx->input[*(ctx->i)] == '"')
		(*(ctx->i))++;
	return (ctx->processed);
}

static void	process_exit_status_outside_quotes(t_proc_ctx *ctx)
{
	char	*exit_str;

	exit_str = ft_itoa(ctx->shell->last_exit);
	if (exit_str)
	{
		ft_strcpy(ctx->processed + *(ctx->proc_len), exit_str);
		*(ctx->proc_len) += ft_strlen(exit_str);
		ft_free(exit_str);
	}
	*(ctx->i) += 2;
}

static char	*process_variable_outside_quotes(t_proc_ctx *ctx)
{
	if (*(ctx->i) + 1 < ctx->end
		&& ctx->input[*(ctx->i) + 1] == '"')
	{
		(*(ctx->i))++;
		return (process_double_quote(ctx));
	}
	else if (*(ctx->i) + 1 < ctx->end
		&& ctx->input[*(ctx->i) + 1] == '\'')
	{
		(*(ctx->i))++;
		return (process_single_quote(ctx));
	}
	else if (*(ctx->i) + 1 < ctx->end
		&& ctx->input[*(ctx->i) + 1] == '?')
		process_exit_status_outside_quotes(ctx);
	else if (*(ctx->i) + 1 < ctx->end
		&& (ft_isalnum(ctx->input[*(ctx->i) + 1])
			|| ctx->input[*(ctx->i) + 1] == '_'))
		process_variable_expansion_outside_quotes(ctx);
	else
		process_literal_dollar(ctx->processed, ctx->proc_len, ctx->i);
	return (ctx->processed);
}

static void	process_token_loop(t_proc_ctx *ctx, int max_len)
{
	while (*(ctx->i) < ctx->end)
	{
		if (ctx->input[*(ctx->i)] == '"')
			process_double_quote(ctx);
		else if (ctx->input[*(ctx->i)] == '\'')
			process_single_quote(ctx);
		else if (ctx->input[*(ctx->i)] == '$')
			process_variable_outside_quotes(ctx);
		else
		{
			if (*(ctx->proc_len) < max_len - 1)
			{
				ctx->processed[*(ctx->proc_len)] = ctx->input[*(ctx->i)];
				(*(ctx->proc_len))++;
			}
			(*(ctx->i))++;
		}
	}
}

char	*process_token_content(const char *input,
			int start, int end, t_shell *shell)
{
	int				max_len;
	char			*processed;
	int				proc_len;
	int				i;
	t_proc_ctx		ctx;

	max_len = calculate_required_memory(input, start, end, shell);
	processed = ft_malloc(max_len);
	proc_len = 0;
	i = start;
	ctx.input = input;
	ctx.i = &i;
	ctx.end = end;
	ctx.processed = processed;
	ctx.proc_len = &proc_len;
	ctx.shell = shell;
	process_token_loop(&ctx, max_len);
	processed[proc_len] = '\0';
	return (processed);
}
