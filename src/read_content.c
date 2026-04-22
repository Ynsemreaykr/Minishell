/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_content.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/29 02:53:39 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/10 10:23:29 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Heredoc sürecinde yazılmış satırları okuma borusundan (pipe okuma ucundan)
** tekrar çeken ve tam teşekküllü bir content dizesi haline sokan yardımcı modül. */

#include "../include/minishell.h"
#include <unistd.h>

/* Genel içeriğe (content) yeni okunan bir satırı (line) ekler.
** String boyutlarına göre malloc yapar ve \n i ekleyerek iade eder. */
static char	*append_line_to_content(char *content, int content_size, char *line)
{
	char	*new_content;
	int		line_len;

	line_len = ft_strlen(line) + 1;
	new_content = ft_malloc(content_size + line_len + 1);
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

/* Pipe içinden yazıldıkça heredoc satırlarını dinamik olarak okur,
** satırlar bittiğinde heredoc tamamlanmış olur ve bütün stringi döndürür. */
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

/* Tek bir satıra okunan yeni harfi birleştirir, eskisini siler.
** read_line_dynamic e hafıza konusunda destekçi bir realloc benzeridir. */
static char	*append_char_to_line(char *line, int line_size, char c)
{
	char	*new_line;

	new_line = ft_malloc(line_size + 2);
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

/* Harf harf pipe tan (heredoctan) satırı okuyarak
** \n i (yenisatırı) görünce okumayı kesip dinamik bir satır stringi döndürür. */
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
