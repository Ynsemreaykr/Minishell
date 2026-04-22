/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort_env.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 02:45:12 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/10 10:23:39 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* env ortam değişkenlerini alfabetik ASCII sırasında sıralamak
** ve export modunda bash ile paralel bir standartla basmak için yazılmış modül. */

#include "../include/minishell.h"

/* Kopyalanmış env dizisine, Bubble Sort algoritmasına benzer bir şekilde,
** alfabetik sıralama uygular. */
void	sort_env(char **env, int count)
{
	int		j;
	int		k;
	char	*tmp;

	j = 0;
	while (j < count - 1)
	{
		k = 0;
		while (k < count - j - 1)
		{
			if (ft_strcmp(env[k], env[k + 1]) > 0)
			{
				tmp = env[k];
				env[k] = env[k + 1];
				env[k + 1] = tmp;
			}
			k++;
		}
		j++;
	}
}

/* Belirtilen env değerini bash standartına uygun formata bürür.
** '_' değişkeni basılmaz. Eğer bir env nin atalı değeri varsa
** declare -x VAR="değer" formatıyla basılır, atalı değilse sadece declare -x VAR olur. */
static void	print_single_env(char *entry)
{
	char	*eq;

	eq = ft_strchr(entry, '=');
	if (eq)
	{
		*eq = '\0';
		if (ft_strcmp(entry, "_") != 0)
		{
			ft_putstr_fd("declare -x ", 1);
			ft_putstr_fd(entry, 1);
			ft_putstr_fd("=\"", 1);
			ft_putstr_fd(eq + 1, 1);
			ft_putstr_fd("\"\n", 1);
		}
		*eq = '=';
	}
	else if (ft_strcmp(entry, "_") != 0)
	{
		ft_putstr_fd("declare -x ", 1);
		ft_putstr_fd(entry, 1);
		ft_putstr_fd("\n", 1);
	}
}

/* Sıralı olarak kaydedilmiş çevresel değişken arrayinin bütününde gezinir,
** her biri için print_single_env'i çağırarak basım yapılmasını sağlar. */
void	print_sorted_env(char **env, int count)
{
	int	j;

	j = 0;
	while (j < count)
	{
		print_single_env(env[j]);
		j++;
	}
}
