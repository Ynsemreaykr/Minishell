/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 00:00:00 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/05 00:00:00 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Libft: İki string içeriğinin birebir aynı olup olmadığını kıyaslar. */

#include "../include/minishell.h"

/* İki string de ardışık harfleri aynı olduğu sürece işaretçileri kaydırır.
** Ayrıldıkları ya da bittikleri yerde ascii farklarını (veya tam eşitlerse 0) döner. */
int	ft_strcmp(const char *s1, const char *s2)
{
	while (*s1 && *s2 && *s1 == *s2)
	{
		s1++;
		s2++;
	}
	return ((unsigned char)*s1 - (unsigned char)*s2);
}
