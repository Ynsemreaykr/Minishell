/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_heredoc.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 17:36:54 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/10 10:23:13 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Komut argv listesindeki yönlendirme ve heredoc token'larını t_redir
** listesine dönüştüren modül. Yönlendirme sonrası argv güncellenir. */

#include "../include/minishell.h"

/* < veya << operatörünü işler:
** - "<" → handle_in_redir ile REDIR_IN düğümü oluşturulur
** - "<<" → delimiter tırnak kontrolü (detect_quote_clean_delimiter),
**   create_heredoc_redir ile HEREDOC düğümü oluşturulur ve cmd'ye eklenir
** - Diğer token'lar → new_argv'ye kopyalanır */
static void	process_input_redir(t_cmd *cmd, int *i, char **new_argv, int *j)
{
	int		quote_type;
	char	*original;
	t_redir	*heredoc_redir;

	if (!ft_strcmp(cmd->argv[*i], "<"))
		handle_in_redir(cmd, cmd->argv, i);
	else if (!ft_strcmp(cmd->argv[*i], "<<"))
	{
		if (is_valid_redir_target(cmd->argv[*i + 1]))
		{
			original = ft_strdup(cmd->argv[*i + 1]);
			quote_type = detect_quote_clean_delimiter(&cmd->argv[*i + 1]);
			heredoc_redir = create_heredoc_redir(original,
					cmd->argv[*i + 1], quote_type);
			if (heredoc_redir)
				add_redir(cmd, heredoc_redir);
			ft_free(original);
			*i += 2;
		}
		else
			*i += 1;
	}
	else
		new_argv[(*j)++] = ft_strdup(cmd->argv[(*i)++]);
}

/* Eski argv'nin tüm elemanlarını serbest bırakır ve new_argv'yi atar.
** argc == 0 ise new_argv de serbest bırakılır ve cmd->argv = NULL yapılır. */
static void	cleanup_and_replace_argv(t_cmd *cmd, char **new_argv, int argc)
{
	int	k;

	k = 0;
	while (cmd->argv && cmd->argv[k])
		ft_free(cmd->argv[k++]);
	ft_free(cmd->argv);
	if (argc == 0)
	{
		ft_free(new_argv);
		cmd->argv = NULL;
	}
	else
		cmd->argv = new_argv;
}

/* argv'nin tüm elemanlarını tarar:
** - ">" veya ">>" → process_output_redir (REDIR_OUT / REDIR_APPEND)
** - "<", "<<" veya normal arg → process_input_redir
** Yönlendirme olmayan token'lar new_argv'ye kopyalanır. */
static void	process_argv_elements(t_cmd *cmd, char **new_argv)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (cmd->argv && cmd->argv[i])
	{
		if (!ft_strcmp(cmd->argv[i], ">") || !ft_strcmp(cmd->argv[i], ">>"))
			process_output_redir(cmd, &i);
		else
			process_input_redir(cmd, &i, new_argv, &j);
	}
	new_argv[j] = NULL;
}

/* Yönlendirme olmayan argüman sayısını hesaplar.
** <, <<, >, >> token'larından sonra gelen geçerli hedef token'ı da
** sayılmaz; bunlar two-token operatör olarak atlanır. */
static int	count_non_redir_args(char **argv)
{
	int	i;
	int	argc;

	i = 0;
	argc = 0;
	while (argv && argv[i])
	{
		if (!ft_strcmp(argv[i], ">") || !ft_strcmp(argv[i], ">>")
			|| !ft_strcmp(argv[i], "<") || !ft_strcmp(argv[i], "<<"))
		{
			if (is_valid_redir_target(argv[i + 1]))
				i += 2;
			else
				i++;
		}
		else
		{
			argc++;
			i++;
		}
	}
	return (argc);
}

/* Komut argv listesindeki yönlendirme token'larını t_redir listesine taşır.
** 1. count_non_redir_args → yeni argv boyutu
** 2. new_argv dizisi oluşturulur
** 3. process_argv_elements → operatörler t_redir'e, argümanlar new_argv'ye
** 4. cleanup_and_replace_argv → eski argv temizlenir, new_argv atanır */
void	parse_redirections_and_heredoc(t_cmd *cmd)
{
	int		argc;
	char	**new_argv;

	argc = count_non_redir_args(cmd->argv);
	new_argv = ft_malloc(sizeof(char *) * (argc + 1));
	process_argv_elements(cmd, new_argv);
	cleanup_and_replace_argv(cmd, new_argv, argc);
}
