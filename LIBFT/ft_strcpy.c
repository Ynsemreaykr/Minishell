/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 00:00:00 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/05 00:00:00 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Libft: Bir stringin tümünü başka bir belleğe kopyalar (üzerine yazar). */

#include "../include/minishell.h"

/* src buffer ının tamamını null görene kadar dest bufferına bit by bit basıp sonuna
** null baytını diker ve dest'i geri iade eder. */
char	*ft_strcpy(char *dest, const char *src)
{
	char	*ptr;

	ptr = dest;
	while (*src)
	{
		*ptr = *src;
		ptr++;
		src++;
	}
	*ptr = '\0';
	return (dest);
}
