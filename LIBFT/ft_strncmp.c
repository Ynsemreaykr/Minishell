/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncmp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 00:10:00 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/05 00:10:00 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Libft: İki stringi, belirtilen 'n' sayısı kadar karaktere kısıtlı bir şekilde kıyaslar. */

#include "../include/minishell.h"

/* Hem n sınırına ulaşılmamışsa, hem diziler devam ediyorsa, hem de harfleri aynıysa ilerler.
** Sınır aşıldıysa eşit (0) kabul eder, aksi takdirde ilk ayrıldıkları ascii farkını döner. */
int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t	i;

	i = 0;
	while (i < n && s1[i] && s2[i] && s1[i] == s2[i])
		i++;
	if (i == n)
		return (0);
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}
