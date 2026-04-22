/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   count_token_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 16:12:21 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/10 10:22:01 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Token sayısıyla ilgili ayrıştırılan farklı token tiplerinin
** (tırnaklı token, yönlendirme, standart kelimeler) sayma yardımcıları. */

#include "../include/minishell.h"

/* Tırnak içerisindeki sözcüğü tek bir token olarak kabul eder.
** Tırnak karakterinin kapandığı yere kadar indeksi ilerletip,
** bu alan için toplam token artış miktarı olan 1 değerini döndürür. */
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

/* Yönlendirmeyi (<, << vb.) bir token olarak kaydeder.
** Kaç karakter olduğunu bularak indeksi ilerletir ve
** yönlendirme tespit edilirse token sayısını artırmak için 1 döner. */
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

/* Sıradan karakterle yazılmış bir kelimeyi (tırnaksız veya ardışık,
** boşluk vb içermeyen) tek token saymak üzere işler.
** Kelime sonlanana kadar (veya boşluk görene kadar) ilerler.
** Bir kelime/token okunmuşsa 1 döndürür. */
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
