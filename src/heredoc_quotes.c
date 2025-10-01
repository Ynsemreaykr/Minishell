/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_quotes.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 09:40:57 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/09/10 10:22:48 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

static void	clean_quotes_from_delimiter(char **delimiter, int len)
{
	char	*delim;
	char	*cleaned;

	delim = *delimiter;
	cleaned = ft_malloc(len - 1);
	ft_strncpy(cleaned, delim + 1, len - 2);
	cleaned[len - 2] = '\0';
	ft_free(*delimiter);
	*delimiter = cleaned;
}

int	detect_quote_clean_delimiter(char **delimiter)
{
	char	*delim;
	int		len;

	if (!*delimiter || !**delimiter)
		return (0);
	delim = *delimiter;
	len = ft_strlen(delim);
	if (len >= 2 && delim[0] == '\'' && delim[len - 1] == '\'')
	{
		clean_quotes_from_delimiter(delimiter, len);
		return (1);
	}
	if (len >= 2 && delim[0] == '"' && delim[len - 1] == '"')
	{
		clean_quotes_from_delimiter(delimiter, len);
		return (2);
	}
	return (0);
}
