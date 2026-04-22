/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtin.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 08:44:22 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/10 10:57:31 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* echo ve 'dot' (.) gibi dahili komutların çalıştırılmasını içeren modül. */

#include "../include/minishell.h"

/* echo komutunun kelimeleri arasında boşluk bırakarak yazar. */
static void	print_echo_args(char **argv, int start)
{
	int	i;

	i = start;
	while (argv[i])
	{
		ft_putstr_fd(argv[i], 1);
		if (argv[i + 1])
			ft_putstr_fd(" ", 1);
		i++;
	}
}

/* Argümanın geçerli bir echo bayrağı "-n" olup olmadığını söyler.
** Birden fazla '-nnn' kullanımına da izin vererek esneklik sağlar. */
static int	is_n_flag(const char *arg)
{
	int	i;

	if (!arg || arg[0] != '-')
		return (0);
	i = 1;
	while (arg[i])
	{
		if (arg[i] != 'n')
			return (0);
		i++;
	}
	return (1);
}

/* echo dahilî komutu.
** İlk argümanları '-n' olduğu sürece flag olarak kabul eder, o indisten sonrasını
** print_echo_args ile yazdırır. n_flag varsa sona \n koymaz. */
int	ft_echo(char **argv)
{
	int	n_flag;
	int	i;

	n_flag = 0;
	i = 1;
	while (argv[i] && is_n_flag(argv[i]))
	{
		n_flag = 1;
		i++;
	}
	print_echo_args(argv, i);
	if (!n_flag)
		ft_putstr_fd("\n", 1);
	return (0);
}

/* '.' dahilî komut (source) uyarı ve kısıtlama modülü. Sadece hata mesajı için var. */
int	ft_dot(char **argv)
{
	if (!argv[1])
	{
		ft_putstr_fd("minishell: .: filename argument required\n", 2);
		return (2);
	}
	return (1);
}
