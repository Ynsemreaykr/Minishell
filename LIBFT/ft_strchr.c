/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 00:00:00 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/05 00:00:00 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Libft: Bir string içinde spesifik bir karakteri arayan işlev modülü. */

#include "../include/minishell.h"

/* İlk eşleştiği yerde stringin o karakterine ait bellek adresini döndürür.
** Eğer aranacak karakter null ise direkt null konumunu (string sonu) döndürür. */
char	*ft_strchr(const char *s, int c)
{
	while (*s)
	{
		if (*s == (char)c)
			return ((char *)s);
		s++;
	}
	if ((char)c == '\0')
		return ((char *)s);
	return (NULL);
}
