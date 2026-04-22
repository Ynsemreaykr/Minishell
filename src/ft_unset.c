/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_unset.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 02:45:12 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/10 10:22:40 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* env ortam değişkenlerini bellekten silen/unset yapan dahilî komut modülü. */

#include "../include/minishell.h"

/* Gönderilen index değerinden itibaren, env dizisinin sonraki elemanlarını
** sola kaydırarak istenilen aradaki elemanın dizin konumunu kapatan fonksiyondur. */
static void	shift_env_vars_left(char **env, int start_index)
{
	int	j;

	j = start_index;
	while (env[j])
	{
		env[j] = env[j + 1];
		j++;
	}
}

/* Verilen isimdeki değişken shell in env listesinde varsa
** tespit eder, yer yer ayrılmış alanı serbest bırakır (ft_free)
** ve shift_env_vars_left ile dizide boşluk bırakmamak adına
** elemanları kaydırır. İşlem başarılı olursa 0, olamazsa -1 döner. */
int	unset_env_var(const char *name, t_shell *shell)
{
	int	name_len;
	int	i;

	if (!shell->env || !name)
		return (-1);
	name_len = ft_strlen(name);
	i = 0;
	while (shell->env[i])
	{
		if ((shell->env[i][name_len] == '=' || shell->env[i][name_len] == '\0')
			&& ft_strncmp(shell->env[i], name, name_len) == 0)
		{
			ft_free(shell->env[i]);
			shift_env_vars_left(shell->env, i);
			return (0);
		}
		i++;
	}
	return (-1);
}

/* unset komut argümanlarıyla çağrılabilen ana yapı. (Örn: unset VAR1 VAR2)
** Girilen tüm argümanları unset_env_var() fonksiyonuna bir döngüde iletir.
** Komut argüman yoksa basitçe 0 dönüp sonlanır. */
int	ft_unset(char **argv, t_shell *shell)
{
	int	i;

	i = 1;
	while (argv[i])
	{
		unset_env_var(argv[i], shell);
		i++;
	}
	return (0);
}
