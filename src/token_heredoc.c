/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_heredoc.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 02:45:12 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/10 10:23:42 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Heredoc delimiter token'ının tokenizer aşamasında okunma modülü.
** << operatörü görüldükten sonra delimiter string'ini token dizisine ekler. */

#include "../include/minishell.h"

/* input[start..end) aralığındaki delimiter string'ini heap'te oluşturur.
** Uzunluk 0 veya negatifse NULL döner. */
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

/* Heredoc delimiter içindeki tırnak karakterlerini işler.
** Tırnak açılış → quote_char ayarla, in_quote = 1
** Tırnak kapanış → in_quote = 0 sıfırla
** Tırnak dışı normal karakter → i ilerlet */
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

/* << operatöründen hemen sonra gelen delimiter token'ını okur ve
** token dizisine (args) ekler.
** - Önce boşluklar atlanır.
** - Boşluk (tırnak dışında) veya başka << görülene kadar devam edilir.
** - Tırnak karakterleri ham olarak dahil edilir (detect_quote_clean_delimiter
**   sonradan tırnakları temizler ve quoted_flag belirler). */
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
