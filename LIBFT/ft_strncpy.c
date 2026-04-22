/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 00:10:00 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/05 00:10:00 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Libft: Kaynak stringten (src), hedef buffer'a belirli bir (n) limitiyle kopyalama. */

#include "../include/minishell.h"

/* Gelen karakter n limitini dolduruncaya ya da src bitinceye dek harf harf kopyalar.
** İşlemin sonucunda limit dolmadı ama src bittiyse dest buffer ının geri kalanını
** null ile (boş) doldurur. */
char	*ft_strncpy(char *dest, const char *src, size_t n)
{
	size_t	i;

	i = 0;
	while (i < n && src[i])
	{
		dest[i] = src[i];
		i++;
	}
	while (i < n)
	{
		dest[i] = '\0';
		i++;
	}
	return (dest);
}
