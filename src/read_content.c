/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_content.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/29 02:53:39 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/08/29 03:05:08 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <readline/readline.h>
#include <fcntl.h>

static char	*append_line_to_content(char *content, int content_size, char *line)
{
	char	*new_content;
	int		line_len;

	line_len = ft_strlen(line) + 1;
	new_content = ft_malloc(content_size + line_len + 1, __FILE__, __LINE__);
	if (!new_content)
	{
		if (content)
			ft_free(content);
		ft_free(line);
		return (NULL);
	}
	if (content)
	{
		ft_strcpy(new_content, content);
		ft_free(content);
	}
	else
		new_content[0] = '\0';
	ft_strcat(new_content, line);
	ft_strcat(new_content, "\n");
	return (new_content);
}

char	*read_heredoc_content(int fd)
{
	char	*content;
	int		content_size;
	char	*line;

	content = NULL;
	content_size = 0;
	line = read_line_dynamic(fd);
	while (line != NULL)
	{
		content = append_line_to_content(content, content_size, line);
		if (!content)
			return (NULL);
		content_size += ft_strlen(line) + 1;
		ft_free(line);
		line = read_line_dynamic(fd);
	}
	if (content_size == 0)
		return (ft_strdup(""));
	return (content);
}

static char	*append_char_to_line(char *line, int line_size, char c)
{
	char	*new_line;

	new_line = ft_malloc(line_size + 2, __FILE__, __LINE__);
	if (!new_line)
	{
		if (line)
			ft_free(line);
		return (NULL);
	}
	if (line)
	{
		ft_strncpy(new_line, line, line_size);
		ft_free(line);
	}
	else
		new_line[0] = '\0';
	new_line[line_size] = c;
	new_line[line_size + 1] = '\0';
	return (new_line);
}

char	*read_line_dynamic(int fd)
{
	char	*line;
	int		line_size;
	char	buffer[1];
	ssize_t	bytes_read;

	line = NULL;
	line_size = 0;
	bytes_read = read(fd, buffer, 1);
	while (bytes_read > 0)
	{
		if (buffer[0] == '\n')
			break ;
		line = append_char_to_line(line, line_size, buffer[0]);
		if (!line)
			return (NULL);
		line_size++;
		bytes_read = read(fd, buffer, 1);
	}
	if (bytes_read == 0)
		return (NULL);
	if (line_size == 0)
		return (ft_strdup(""));
	return (line);
}
