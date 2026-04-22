/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/29 01:08:56 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/10 10:23:50 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Ana tokenizer modülü.
** Bir komut segmentini (pipe sonrası tek komut string'ini) token'lara böler.
** Tırnak ve değişken genişletme farkındalığıyla çalışır. */

#include "../include/minishell.h"

/* Token dizisini başlatır.
** token_count <= 0 ise tek elemanlı boş dizi döner (should_return=1).
** Aksi hâlde token_count+1 boyutlu boş dizi döner. */
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

/* Yönlendirme token'ını işler (<, >, <<, >>).
** get_redirection_length ile uzunluk hesaplanır, string kopyalanır.
** << operatörü sonrasında heredoc delimiter token'ı da okunur. */
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

/* Token bölme döngüsü.
** Her iterasyonda bir token okunur:
** - Boşluklar atlanır (skip_whitespace)
** - Yönlendirme operatörü → handle_redirection_token
** - Normal/tırnaklı token → process_regular_quoted_token */
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

/* Ana tokenize fonksiyonu.
** count_tokens ile kaç token olduğu öğrenilir, dizi hazırlanır,
** process_token_loop ile tokenler doldurulur, NULL ile sonlandırılır.
** Tırnak içi ve dışı değişken genişletme bu süreçte gerçekleşir. */
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

/* Tek tırnak içindeki içeriği olduğu gibi (genişletme yapmadan) kopyalar.
** ctx->i açılış tırnağından sonrayı gösterir; kapanış tırnağına kadar
** tüm karakterler ctx->processed tamponuna yazılır.
** Kapanış tırnağı varsa i ilerletilir. */
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
