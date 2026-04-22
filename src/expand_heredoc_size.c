/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_heredoc_size.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 09:27:38 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/10 10:22:31 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Heredoc okuması sırasında değişkenin genişletilmiş halinin
** tam olarak ne kadar byte yer tutacağını hesaplama fonksiyonları. */

#include "../include/minishell.h"

/* Satırdaki konumdan başlayarak ortam değişkeni ismini ayrıştırır,
** ismin string halini oluşturup (malloc) döndürür. */
static char	*get_var_name_from_line(const char *line, int *i)
{
	int		start;
	int		len;
	char	*var_name;

	start = *i;
	while (line[*i] && (ft_isalnum(line[*i]) || line[*i] == '_'))
		(*i)++;
	len = *i - start;
	if (len == 0)
		return (NULL);
	var_name = ft_malloc(len + 1);
	ft_strncpy(var_name, line + start, len);
	var_name[len] = '\0';
	return (var_name);
}

/* Ayrıştırılan değişkenin env listesinde değerini bulup
** ortam değişkeninin uzunluğunu (byte cinsinden) döndürür. */
static int	get_var_size(const char *line, int *i, t_shell *shell)
{
	char	*var_name;
	char	**envp;
	int		e;
	int		size;

	size = 0;
	var_name = get_var_name_from_line(line, i);
	if (!var_name)
		return (0);
	envp = get_env(shell);
	e = 0;
	while (envp && envp[e])
	{
		if (!ft_strncmp(envp[e], var_name, ft_strlen(var_name))
			&& envp[e][ft_strlen(var_name)] == '=')
		{
			size = ft_strlen(envp[e] + ft_strlen(var_name) + 1);
			break ;
		}
		e++;
	}
	ft_free(var_name);
	return (size);
}

/* Sadece boyutu ve indeksi birer ilerletir (normal karakterler için). */
static void	increment_size_and_index(int *size, int *i)
{
	(*size)++;
	(*i)++;
}

/* $ sembolünden başlayan kısmın boyut hesabını yapar.
** $? ise çıkış kodu uzunluğu, $VAR ise değişken boyutu olarak yansıtır. */
static int	dollar_expansion_size(const char *line, int *i, t_shell *shell)
{
	int	size;

	size = 0;
	(*i)++;
	if (line[*i] == '?')
	{
		size += ft_strlen(ft_itoa(shell->last_exit));
		(*i)++;
	}
	else if (ft_isalnum(line[*i]) || line[*i] == '_')
		size += get_var_size(line, i, shell);
	else
		increment_size_and_index(&size, i);
	return (size);
}

/* Tüm heredoc satırının (genişletilmeler dahil) boyutunu
** malloc edebilmek için byte hesabı gerçekleştirir. */
int	calculate_expansion_size(const char *line, t_shell *shell)
{
	int	size;
	int	i;

	size = 0;
	i = 0;
	while (line[i])
	{
		if (line[i] == '$')
			size += dollar_expansion_size(line, &i, shell);
		else
			increment_size_and_index(&size, &i);
	}
	return (size);
}
