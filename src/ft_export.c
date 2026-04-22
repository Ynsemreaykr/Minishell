/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_export.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 16:23:20 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/10 10:22:37 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* export (çevresel değişken bildirme) komutunun ana destek ve argümansız
** çalıştırma kısımları. */

#include "../include/minishell.h"

/* Gelen ortam değişkeni isminin geçerli karakterler içerip içermediğini kontrol eder.
** İsim harf veya alt çizgi ile başlamalıdır, rakamla başlayamaz.
** Kalan kısmı alfanümerik veya alt çizgi olabilir. Belirtilen bu kurallara uyuyorsa
** 1, uymuyorsa 0 döner. */
int	is_valid_identifier(const char *name)
{
	int	i;

	if (!name[0] || (!ft_isalpha(name[0]) && name[0] != '_'))
		return (0);
	i = 1;
	while (name[i])
	{
		if (!ft_isalnum(name[i]) && name[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

/* Girilen argümanın içerisinden sadece isim bölümünü,
** `=` (eşittir) işaretine kadarki alanı koparıp döndürür.
** Eğer `=` yoksa (sadece export VAR yapılmışsa) tamamını isim kabul eder. */
char	*extract_var_name(const char *arg)
{
	char	*eq;
	char	*name;
	int		len;

	eq = ft_strchr(arg, '=');
	if (eq)
	{
		len = eq - arg;
		name = ft_malloc(len + 1);
		ft_strncpy(name, arg, len);
		name[len] = '\0';
	}
	else
		name = ft_strdup(arg);
	return (name);
}

/* Hiçbir argüman verilmeden sadece `export` yazıldığında ortam değişkenlerinin
** ASCII sırasına (alfabetik olarak) göre nasıl yazdırılacağını belirler.
** Mevcut env dizisi geçici olarak kopyalanır, sort_env() ile alfabetik sıralanır,
** print_sorted_env() ile istenen formatta ("declare -x ...") basılır ve sökülür. */
int	handle_no_args_export(t_shell *shell)
{
	char	**env;
	char	**sorted_env;
	int		count;
	int		j;

	env = get_env(shell);
	count = 0;
	while (env && env[count])
		count++;
	sorted_env = ft_malloc(sizeof(char *) * (count + 1));
	j = 0;
	while (j < count)
	{
		sorted_env[j] = ft_strdup(env[j]);
		j++;
	}
	sorted_env[count] = NULL;
	sort_env(sorted_env, count);
	print_sorted_env(sorted_env, count);
	j = 0;
	while (j < count)
		ft_free(sorted_env[j++]);
	ft_free(sorted_env);
	return (0);
}
