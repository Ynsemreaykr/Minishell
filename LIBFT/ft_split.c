/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 17:58:00 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/09/05 17:58:00 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

static int	count_words(const char *s, char c)
{
	int	count;
	int	in_word;

	count = 0;
	in_word = 0;
	while (*s)
	{
		if (*s != c && !in_word)
		{
			in_word = 1;
			count++;
		}
		else if (*s == c)
			in_word = 0;
		s++;
	}
	return (count);
}

static char	*get_next_word(const char **s, char c)
{
	const char	*start;
	int			len;
	char		*word;

	len = 0;
	while (**s == c)
		(*s)++;
	start = *s;
	while (**s && **s != c)
	{
		len++;
		(*s)++;
	}
	if (len == 0)
		return (NULL);
	word = ft_malloc(len + 1);
	ft_strncpy(word, start, len);
	word[len] = '\0';
	return (word);
}

char	**ft_split(const char *s, char c)
{
	int			word_count;
	char		**result;
	int			i;
	const char	*ptr;
	char		*word;

	if (!s)
		return (NULL);
	word_count = count_words(s, c);
	result = ft_malloc(sizeof(char *) * (word_count + 1));
	i = 0;
	ptr = s;
	while (i < word_count)
	{
		word = get_next_word(&ptr, c);
		if (word)
			result[i++] = word;
		else
			break ;
	}
	result[i] = NULL;
	return (result);
}

void	ft_split_free(char **split_result)
{
	int	i;

	if (!split_result)
		return ;
	i = 0;
	while (split_result[i])
	{
		ft_free(split_result[i]);
		i++;
	}
	ft_free(split_result);
}
