/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 00:10:00 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/05 00:10:00 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Libft: Verilen stringin dinamik bellekte tam ölçülü bir kopyasını oluşturur. */

#include "../include/minishell.h"

/* Gelen stringin boyunu ft_strlen ile alıp null payıyla birlikte ft_malloc ile
** hafızadan yer tahsis eder. Sonra ft_strcpy yardımıyla yazdırıp kopyayı döndürür. */
char	*ft_strdup(const char *s)
{
	char	*dup;

	if (!s)
		return (NULL);
	dup = ft_malloc(ft_strlen(s) + 1);
	if (!dup)
		return (NULL);
	ft_strcpy(dup, s);
	return (dup);
}
