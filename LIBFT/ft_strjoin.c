/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 00:10:00 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/05 00:10:00 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Libft: İki ayrı string dizisini birleştirerek yeni bir string üretir. */

#include "../include/minishell.h"

/* Gelen s1 ve s2 nin null durumlarını kontrol eder, ikisi de doluysa boylarını
** hesaplayıp null payıyla birlikte ft_malloc atar. Sonrasında strcpy ve strcat
** kullanarak ikisini sırayla kopyalar ve döndürür. */
char	*ft_strjoin(const char *s1, const char *s2)
{
	char	*result;
	int		len1;
	int		len2;

	if (!s1 && !s2)
		return (ft_strdup(""));
	if (!s1)
		return (ft_strdup(s2));
	if (!s2)
		return (ft_strdup(s1));
	len1 = ft_strlen(s1);
	len2 = ft_strlen(s2);
	result = ft_malloc(len1 + len2 + 1);
	if (!result)
		return (NULL);
	ft_strcpy(result, s1);
	ft_strcat(result, s2);
	return (result);
}
