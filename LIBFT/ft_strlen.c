/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlen.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 00:10:00 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/05 00:10:00 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Libft: Gelen stringin karakter uzunluğunu (null haricinde) hesaplar. */

#include "../include/minishell.h"

/* Bitiş baytına (null) kadar ilerler ve kaç karakter atladıysa onu geri döndürür. */
size_t	ft_strlen(const char *s)
{
	size_t	len;

	len = 0;
	while (s[len])
		len++;
	return (len);
}
