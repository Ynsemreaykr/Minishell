/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_check.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 17:36:54 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/10 10:23:18 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Pipe operatörü sözdizimi doğrulama modülü.
** Başta/sonda pipe, ardarda pipe ve tırnak içindeki pipe tespiti. */

#include "../include/minishell.h"

/* Belirtilen pozisyonun tırnak içinde olup olmadığını kontrol eder.
** 0'dan pos'a kadar tırnak durumu takip edilir.
** Tırnak içinde → 1, dışında → 0 döner. */
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

/* Pipe operatörleri sözdizimini doğrular:
** - Giriş sadece boşluklardan sonra | ile başlıyorsa → hata
** - Giriş | ile bitiyorsa → hata
** - İki ardışık tırnak dışı || varsa → hata
** Geçerli → 1, hatalı → 0 döner. */
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

/* Tırnak içindeki pipe'lar hariç gerçek pipe sayısını sayar.
** count_pipes sonucu split_by_pipes'ta dizi boyutu hesaplamak için kullanılır. */
int	count_pipes(const char *input)
{
	int	pipe_count;
	int	i;
	int	in_single_quote;
	int	in_double_quote;

	pipe_count = 0;
	i = 0;
	in_single_quote = 0;
	in_double_quote = 0;
	while (input[i])
	{
		toggle_quote_state(input[i], &in_single_quote, &in_double_quote);
		if (input[i] == '|' && !in_single_quote && !in_double_quote)
			pipe_count++;
		i++;
	}
	return (pipe_count);
}

/* Girişin pipe sözdizimini doğrular.
** Geçersizse hata mesajı yazar ve 0 döndürür, geçerliyse 1 döndürür. */
int	validate_input_and_pipes(const char *input)
{
	if (!pipe_check(input))
	{
		ft_putstr_fd(
			"minishell: syntax error near unexpected token `|'\n", 2);
		return (0);
	}
	return (1);
}
