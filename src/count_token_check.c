/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   count_token_check.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 16:12:21 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/10 10:21:57 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Komut stringinin içinde kaç tane token (kelime/operatör)
** olduğunu sayan yardımcı fonksiyonların bulunduğu modül.
** Token dizisi malloc edilecekken kullanılacak eleman sayısını hesaplar. */

#include "../include/minishell.h"

/* Karakterin yönlendirme operatörü olup olmadığını ve
** tek ya da çift karakter (< vs << veya > vs >>)  olup olmadığını anlar.
** Uzunluğu (1 veya 2) olarak döndürür. Eğer değilse 0 döner. */
int	get_redirection_length(const char *input, int pos)
{
	if (input[pos] == '<')
	{
		if (input[pos + 1] == '<')
			return (2);
		return (1);
	}
	if (input[pos] == '>')
	{
		if (input[pos + 1] == '>')
			return (2);
		return (1);
	}
	return (0);
}

/* İstenilen pozisyondaki karakterin sadece yönlendirme operatörü
** başlangıcı olup olmadığını kontrol eder. Evetse 1, hayırsa 0 döner. */
int	check_redirection_operator(const char *input, int pos)
{
	if (input[pos] == '<' || input[pos] == '>')
	{
		if ((input[pos] == '<' && input[pos + 1] == '<')
			|| (input[pos] == '>' && input[pos + 1] == '>'))
			return (1);
		return (1);
	}
	return (0);
}

/* Boşlukları (Space ve Tab) atlayarak geçerli ilk karakterin
** dizinini döndürür. */
int	skip_whitespace(const char *input, int pos)
{
	while (input[pos] == ' ' || input[pos] == '\t')
		pos++;
	return (pos);
}

/* Sayma işlemi sırasında tırnak içini bütün tek bir token olarak varsayar,
** bu yüzden tırnak durumunu açık-kapalı olarak günceller. */
void	update_quote_state(const char *input, int *pos,
			int *in_quote, char *quote_char)
{
	if ((input[*pos] == '"' || input[*pos] == '\'') && !*in_quote)
	{
		*quote_char = input[*pos];
		*in_quote = 1;
		(*pos)++;
	}
	else if (input[*pos] == *quote_char && *in_quote)
	{
		*in_quote = 0;
		*quote_char = 0;
		(*pos)++;
	}
	else
		(*pos)++;
}

/* Verilen komut dizesinde (tırnaklar ve işaretler hesaba katılarak)
** toplamda kaç tane geçerli token olduğunu hesaplar. */
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
