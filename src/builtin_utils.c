/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 08:18:16 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/10 10:21:28 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Dahili (builtin) komutlara destek sağlayan ek yardımcı fonksiyonların olduğu modül.
** cd, env gibi işlevlerin alt işlemleri bulunur. */

#include "../include/minishell.h"
#include <unistd.h>

/* cd komutunun hedef dizin argümanını ayrıştırır.
** Sadece 'cd' yazıldıysa veya argüman verilmediyse HOME env sine gidilecek yolu sağlar.
** Çok fazla argüman varsa null döndürüp hata basar. */
static const char	*cd_get_path(char **argv, int argc)
{
	const char	*path;

	if (argc > 2)
	{
		ft_putstr_fd("cd: too many arguments\n", 2);
		return (NULL);
	}
	if (argv[1])
	{
		if (ft_strlen(argv[1]) == 0)
			return ("");
		path = argv[1];
	}
	else
		path = getenv("HOME");
	if (!path)
	{
		ft_putstr_fd("cd: HOME not set\n", 2);
		return (NULL);
	}
	return (path);
}

/* cd (change directory) dahilî komutu.
** Eski çalışma dizinini (getcwd) bulur ve OLDPWD içerisine kaydeder.
** chdir() sistem çağırısıyla konumu değiştirir.
** Başarılı olursa yeni konumu tekrar bulur ve PWD içerisine setler. */
int	ft_cd(char **argv, t_shell *shell)
{
	int			argc;
	const char	*path;
	char		current_dir[1024];
	char		new_dir[1024];

	argc = 0;
	while (argv[argc])
		argc++;
	path = cd_get_path(argv, argc);
	if (!path)
		return (1);
	if (ft_strlen(path) == 0)
		return (0);
	if (getcwd(current_dir, sizeof(current_dir)) != NULL)
		set_env_var("OLDPWD", current_dir, shell);
	if (chdir(path) != 0)
	{
		ft_putstr_fd("minishell: cd: ", 2);
		ft_putstr_fd(path, 2);
		ft_putstr_fd(": No such file or directory\n", 2);
		return (1);
	}
	if (getcwd(new_dir, sizeof(new_dir)) != NULL)
		set_env_var("PWD", new_dir, shell);
	return (0);
}

/* Çevresel değişkenlerin (environment variables) hepsini stdout'a yazar.
** (Sadece değeri olan, '=' içeren değişkenler yazdırılır. Örn export A yapılınca
** env'de gözükmemelidir; sadece export komutunda gözükür.) */
int	ft_env(t_shell *shell)
{
	char	**env;
	int		i;

	env = get_env(shell);
	i = 0;
	while (env && env[i])
	{
		if (ft_strchr(env[i], '='))
		{
			ft_putstr_fd(env[i], 1);
			ft_putstr_fd("\n", 1);
		}
		i++;
	}
	return (0);
}

/* Kabuğun env değişkenlerini barındıran dizinin pointerını getirir. */
char	**get_env(t_shell *shell)
{
	return (shell->env);
}
