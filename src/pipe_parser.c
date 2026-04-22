/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_parser.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 15:49:44 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/10 10:23:21 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Pipe operatörüne göre girişi komut segmentlerine bölen modül.
** Tırnak içindeki pipe karakterleri operatör olarak sayılmaz. */

#include "../include/minishell.h"

/* Tırnak durumunu geçer:
** - Tek tırnak (') çift tırnak içinde değilse → toggle
** - Çift tırnak (") tek tırnak içinde değilse → toggle */
void	toggle_quote_state(char c, int *in_single_quote, int *in_double_quote)
{
	if (c == '\'' && !*in_double_quote)
		*in_single_quote = !*in_single_quote;
	else if (c == '"' && !*in_single_quote)
		*in_double_quote = !*in_double_quote;
}

/* Giriş dizisinin [start, end) aralığına karşılık gelen string'i
** heap'te oluşturup döndürür. end <= start ise NULL döner. */
static char	*extract_command_string(const char *input, int start, int end)
{
	int		len;
	char	*command;

	if (end <= start)
		return (NULL);
	len = end - start;
	command = ft_malloc(len + 1);
	ft_strncpy(command, input + start, len);
	command[len] = '\0';
	return (command);
}

/* Girişte bir sonraki tırnak dışı pipe operatörünü arar.
** Pipe bulununca ilgili komut segmenti string olarak döndürülür.
** start güncellenerek sonraki segmentin nereden başlayacağı işaretlenir.
** Daha fazla pipe yoksa NULL döner. */
static char	*parse_pipe_loop(const char *input, int *start, int *i)
{
	int		in_single_quote;
	int		in_double_quote;
	char	*command_string;

	in_single_quote = 0;
	in_double_quote = 0;
	while (input[*i])
	{
		toggle_quote_state(input[*i], &in_single_quote, &in_double_quote);
		if (input[*i] == '|' && !in_single_quote && !in_double_quote)
		{
			command_string = extract_command_string(input, *start, *i);
			*start = (*i) + 1;
			return (command_string);
		}
		(*i)++;
	}
	return (NULL);
}

/* Komut string'ini commands dizisine ekler ve sayacı artırır. */
static void	add_pipe_command(char **commands, int *cmd_count, char *cmd)
{
	commands[*cmd_count] = cmd;
	(*cmd_count)++;
}

/* Girişi pipe operatörlerine göre böler ve string dizisi olarak döndürür.
** Tırnak içindeki | karakterleri görmezden gelinir.
** count → toplam komut sayısı (pipe_count + 1)
** Örnek: "ls | grep .c | wc -l" → ["ls ", " grep .c ", " wc -l"] */
char	**split_by_pipes(const char *input, int *count)
{
	char	**commands;
	int		cmd_count;
	int		start;
	int		i;
	char	*command_string;

	commands = ft_malloc(sizeof(char *) * (count_pipes(input) + 2));
	cmd_count = 0;
	start = 0;
	i = 0;
	command_string = parse_pipe_loop(input, &start, &i);
	while (command_string)
	{
		add_pipe_command(commands, &cmd_count, command_string);
		i++;
		command_string = parse_pipe_loop(input, &start, &i);
	}
	command_string = extract_command_string(input, start, i);
	if (command_string)
		add_pipe_command(commands, &cmd_count, command_string);
	commands[cmd_count] = NULL;
	*count = cmd_count;
	return (commands);
}
