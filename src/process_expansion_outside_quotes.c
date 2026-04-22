/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_expansion_outside_quotes.c                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 01:55:00 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/10 10:23:26 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Tırnakların dışında kalan ortam değişkeni kullanımlarının ($VAR)
** genişletilmesi için (expansion) yazılan yardımcı modül. */

#include "../include/minishell.h"

/* Tırnak dışarısında bulunmuş ortam değişkeninin karşılığını arar.
** Ortam değişkeni değeri varsa işlenmiş (processed) tampona kopyalar. */
static void	handle_env_variable_case_outside(t_proc_ctx *ctx, int name_start)
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

/* Tırnak dışarısındaki $VAR ifadesindeki isim bölümünü okur
** ve handle_env_variable_case_outside ile yerine orijinal içeriği yazar. */
void	process_variable_expansion_outside_quotes(t_proc_ctx *ctx)
{
	int	name_start;

	(*(ctx->i))++;
	name_start = *(ctx->i);
	while (*(ctx->i) < ctx->end
		&& (ft_isalnum(ctx->input[*(ctx->i)])
			|| ctx->input[*(ctx->i)] == '_')
		&& ctx->input[*(ctx->i)] != ' '
		&& ctx->input[*(ctx->i)] != '\t'
		&& ctx->input[*(ctx->i)] != '"'
		&& ctx->input[*(ctx->i)] != '\'')
		(*(ctx->i))++;
	handle_env_variable_case_outside(ctx, name_start);
}
