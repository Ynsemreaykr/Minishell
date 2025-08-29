/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/29 02:36:07 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/08/29 03:07:46 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static void	int_to_str(int num, char *str)
{
	int	i;
	int	temp;
	int	digits;

	i = 0;
	temp = num;
	digits = 0;
	if (temp == 0)
		digits = 1;
	else
	{
		while (temp != 0)
		{
			temp /= 10;
			digits++;
		}
	}
	str[digits] = '\0';
	i = digits - 1;
	while (i >= 0)
	{
		str[i] = '0' + (num % 10);
		num /= 10;
		i--;
	}
}

int	is_redirection_operator(const char *s, int i, int r_index)
{
	if (s[i] == '>' || s[i] == '<')
	{
		if (r_index == 0)
			return (1);
		return (2);
	}
	return (0);
}

int	is_exit_status_var(const char *s, int i, int r_index)
{
	if (s[i] == '$' && s[i + 1] == '?')
	{
		if (r_index == 0)
			return (1);
		return (2);
	}
	return (0);
}

static void	process_quote_content(const char *s, int *i,
	char *res, int *r_index)
{
	char	quote;

	quote = s[*i];
	res[(*r_index)++] = s[(*i)++];
	while (s[*i] && s[*i] != quote)
		res[(*r_index)++] = s[(*i)++];
	if (s[*i] == quote)
		res[(*r_index)++] = s[(*i)++];
}

void	handle_quotes_with_flags(const char *s, int *i, char *res, int *r_index)
{
	char	quote;
	int		*flags;

	quote = s[*i];
	flags = (int *)(r_index + 1);
	if (quote == '\'')
		*flags &= ~1;
	*flags |= 4;
	process_quote_content(s, i, res, r_index);
	if (s[*i - 1] != quote)
		*flags |= 2;
}

void	extract_variable_name(const char *token, int var_start, char *varname)
{
	int	var_end;

	var_end = var_start;
	if (token[var_start] == '?' || token[var_start] == '_')
		var_end = var_start + 1;
	else if (token[var_start] == '"' || token[var_start] == '\''
		|| token[var_start] == '\0')
		var_end = var_start;
	else
	{
		while (token[var_end] && (ft_isalnum(token[var_end])
				|| token[var_end] == '_') && token[var_end] != '\''
			&& token[var_end] != '"')
			var_end++;
	}
	ft_strncpy(varname, token + var_start, var_end - var_start);
	varname[var_end - var_start] = '\0';
}

void	expand_exit_status(char *buf, int *b, int last_exit)
{
	char	numbuf[16];

	int_to_str(last_exit, numbuf);
	ft_strcat(buf, numbuf);
	*b += ft_strlen(numbuf);
}

void	expand_last_arg(char *buf, int *b)
{
	(void)buf;
	(void)b;
}

void	expand_env_variable(char *buf, int *b, const char *varname,
							char **envp)
{
	int	e;

	e = 0;
	while (envp && envp[e])
	{
		if (!ft_strncmp(envp[e], varname, ft_strlen(varname))
			&& envp[e][ft_strlen(varname)] == '=')
		{
			ft_strcat(buf, envp[e] + ft_strlen(varname) + 1);
			*b += ft_strlen(envp[e] + ft_strlen(varname) + 1);
			break ;
		}
		e++;
	}
}

char	**resize_args_array(char **args, int *argc, int split_count)
{
	char	**new_args;
	int		k;

	if (*argc + split_count > 0 && (*argc + split_count) % 32 == 0)
	{
		new_args = ft_malloc(sizeof(char *) * (*argc + split_count + 34),
				__FILE__, __LINE__);
		k = 0;
		while (k < *argc)
		{
			new_args[k] = args[k];
			k++;
		}
		ft_free(args);
		return (new_args);
	}
	return (args);
}
