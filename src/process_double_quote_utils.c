/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_double_quote_utils.c                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 10:13:17 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/10 10:23:24 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Çift tırnak içinde bulunan değişkenlerin ve $? ifadesinin
** genişletilmesi için yardımcı fonksiyonları içeren modül. */

#include "../include/minishell.h"

/* Çıkış kodunu ($?) genişleterek string'e ekler.
** Kod string'e çevrilir (ft_itoa) ve processed buffer'ına kopyalanır. */
void	process_exit_status(int code, char *processed, int *proc_len, int *i)
{
	char	*exit_str;

	exit_str = ft_itoa(code);
	if (exit_str)
	{
		ft_strcpy(processed + *proc_len, exit_str);
		*proc_len += ft_strlen(exit_str);
		ft_free(exit_str);
	}
	*i += 2;
}

/* Sadece $ işareti bulunduğunda (ardından karakter vs yoksa)
** onu normal bir karakter (literal) olarak string'e yazar. */
void	process_literal_dollar(char *processed, int *proc_len, int *i)
{
	processed[*proc_len] = '$';
	(*proc_len)++;
	(*i)++;
}

/* Özel bir anlamı olmayan normal karakterleri
** sırayla çıkış dizesine kopyalar. */
void	process_regular_char(const char *input, int *i, char *processed,
			int *proc_len)
{
	processed[*proc_len] = input[*i];
	(*proc_len)++;
	(*i)++;
}

/* Ortam değişkeni adını çıkardıktan sonra env dizisinde karşılığını arar.
** Varsa, o değeri çıkış buffer'ına ekler. */
static void	handle_env_variable_case(t_proc_ctx *ctx, int name_start)
{
	int		name_length;
	char	*env_var_name;
	char	*env_value;
	int		value_len;

	name_length = *(ctx->i) - name_start;
	if (name_length > 0)
	{
		env_var_name = ft_malloc(name_length + 1);
		ft_strncpy(env_var_name, ctx->input + name_start, name_length);
		env_var_name[name_length] = '\0';
		env_value = get_env_var(env_var_name, ctx->shell);
		if (env_value)
		{
			value_len = ft_strlen(env_value);
			ft_strcpy(ctx->processed + *(ctx->proc_len), env_value);
			*(ctx->proc_len) += value_len;
		}
		ft_free(env_var_name);
	}
}

/* Çift tırnaklar içindeki ortam değişkenlerini okur,
** ardından genişletmek üzere handle_env_variable_case'i çağırır. */
void	process_variable_expansion_in_quotes(t_proc_ctx *ctx)
{
	int	name_start;

	(*(ctx->i))++;
	name_start = *(ctx->i);
	while (*(ctx->i) < ctx->end && ctx->input[*(ctx->i)] != '"'
		&& (ft_isalnum(ctx->input[*(ctx->i)])
			|| ctx->input[*(ctx->i)] == '_'))
		(*(ctx->i))++;
	handle_env_variable_case(ctx, name_start);
}
