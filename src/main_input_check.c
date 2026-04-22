/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_input_check.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 17:15:37 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/10 10:23:00 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Kullanıcıdan gelen ham giriş dizisinin sözdizimi kontrolünü yapan modül.
** Tırnak çifti doğrulama, yönlendirme operatörü doğrulama ve
** yalnızca boşluklardan oluşan giriş tespiti burada gerçekleşir. */

#include "../include/minishell.h"

/* Tırnak durumunu günceller (tek veya çift tırnak açma/kapama).
** in_quote = 0 iken tırnak görülürse → tırnak açılır, quote_char ayarlanır.
** in_quote = 1 iken eşleşen tırnak görülürse → tırnak kapanır. */
static void	update_quote_flags(char c, int *in_quote, char *quote_char)
{
	if (*in_quote == 0 && (c == '"' || c == '\''))
	{
		*quote_char = c;
		*in_quote = 1;
	}
	else if (*in_quote && c == *quote_char)
	{
		*in_quote = 0;
		*quote_char = 0;
	}
}

/* Yönlendirme operatörü (< >) sözdizimini doğrular.
** Tırnak dışındaki her < veya > için check_redir_operator() çağrılır.
** Giriş sonunda tek başına < veya > varsa sözdizimi hatası verilir.
** Geçerli → 1, hatalı → 0 döndürür. */
static int	check_redirection_syntax(const char *input)
{
	int		i;
	int		in_quote;
	char	quote_char;

	i = 0;
	in_quote = 0;
	quote_char = 0;
	while (input[i])
	{
		update_quote_flags(input[i], &in_quote, &quote_char);
		if (!in_quote && (input[i] == '>' || input[i] == '<'))
		{
			if (!check_redir_operator(input, &i))
				return (0);
		}
		else
			i++;
	}
	if (i > 0 && (input[i - 1] == '>' || input[i - 1] == '<'))
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
		ft_putstr_fd("newline'\n", 2);
		return (0);
	}
	return (1);
}

/* Girişin tamamen boşluk (space/tab) karakterlerinden oluşup oluşmadığını
** kontrol eder. Tamamen boşluksa 1, değilse 0 döndürür. */
static int	is_only_whitespace(const char *input)
{
	int	i;

	i = 0;
	while (input[i])
	{
		if (input[i] != ' ' && input[i] != '\t')
			return (0);
		i++;
	}
	return (1);
}

/* Girişteki tırnak çiftlerinin kapalı olup olmadığını doğrular.
** Tek tırnak ve çift tırnak sayacı tutulur; giriş biterken
** herhangi biri hâlâ açıksa uygun hata mesajı yazılır ve 0 döndürülür.
** Tüm tırnaklar kapalıysa 1 döndürülür. */
static int	check_quotes(const char *input)
{
	int	in_single_quote;
	int	in_double_quote;
	int	i;

	in_single_quote = 0;
	in_double_quote = 0;
	i = 0;
	while (input[i])
	{
		if (input[i] == '\'' && !in_double_quote)
			in_single_quote = !in_single_quote;
		else if (input[i] == '"' && !in_single_quote)
			in_double_quote = !in_double_quote;
		i++;
	}
	if (in_single_quote)
		ft_putstr_fd("minishell: syntax error: unclosed single quote\n", 2);
	else if (in_double_quote)
		ft_putstr_fd("minishell: syntax error: unclosed double quote\n", 2);
	return (!(in_single_quote || in_double_quote));
}

/* Ham girişin işlenmeye hazır olup olmadığını kontrol eder.
** Sırasıyla:
**   1. Yalnızca boşluksa → 0 döndür (işlem yapma)
**   2. Kapatılmamış tırnak varsa → last_exit=2, 0 döndür
**   3. Hatalı yönlendirme sözdizimi varsa → last_exit=2, 0 döndür
** Tüm kontroller geçilirse → 1 döndür (giriş geçerli) */
int	check_input(const char *input, t_shell *shell)
{
	if (is_only_whitespace(input))
		return (0);
	if (!check_quotes(input))
	{
		shell->last_exit = 2;
		return (0);
	}
	if (!check_redirection_syntax(input))
	{
		shell->last_exit = 2;
		return (0);
	}
	return (1);
}
