/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 10:47:05 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/10 10:23:05 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Minishell ana giriş dosyası.
** Shell başlatma, readline ile giriş okuma ve ana döngüyü yönetir.
** Tüm proje bu dosyadaki main() fonksiyonundan başlar. */

#include "../include/minishell.h"
#include <readline/readline.h>
#include <readline/history.h>

/* Global sinyal değişkeni: sinyal işleyicilerinden main döngüsüne
** sinyal numarasını iletmek için kullanılır (sig_atomic_t = thread-safe) */
sig_atomic_t	g_signal_number = 0;

/* Shell yapısını ilk değerleriyle hazırlar:
** - last_exit = 0 (başlangıçta hata yok)
** - Ortam değişkenlerini envp'den kopyalar (init_env)
** - SHLVL ortam değişkenini bir artırır (update_shlvl)
** - Normal sinyal işleyicilerini kurar (Ctrl+C, Ctrl+\) */
static t_shell	init_shell(char **envp)
{
	t_shell	shell;

	shell.last_exit = 0;
	shell.env = NULL;
	init_env(envp, &shell);
	update_shlvl(&shell);
	setup_normal_signals();
	return (shell);
}

/* Kullanıcıdan alınan girişi ön-işlemden geçirir:
** - NULL ise (Ctrl+D / EOF) → shell'den çık, 0 döndür
** - "exit" ise → belleği temizle, 0 döndür
** - check_input başarısız ise (sözdizimi hatası) → 1 döndür
** - Geçerli giriş ise → 2 döndür (komut işleme aşamasına geç) */
static int	handle_input(char *input, t_shell *shell)
{
	if (!input)
		return (0);
	if (!ft_strcmp(input, "exit"))
	{
		ft_putstr_fd("exit\n", 1);
		ft_free(input);
		return (0);
	}
	if (!check_input(input, shell))
	{
		ft_free(input);
		return (1);
	}
	return (2);
}

/* readline() ile kullanıcıdan giriş okur.
** - NULL dönerse (Ctrl+D) "exit" yazar ve NULL döner
** - Giriş boş değilse GNU readline geçmiş listesine ekler */
static char	*get_user_input(void)
{
	char	*input;

	input = readline("minishell$ ");
	if (!input)
	{
		ft_putstr_fd("exit\n", 1);
		return (NULL);
	}
	if (*input)
		add_history(input);
	return (input);
}

/* Shell yapısını hazırlar; argüman kontrolü yapar.
** argc > 1 ise (minishell bir argümanla çağrıldıysa) hata mesajı yazıp çıkar.
** Aksi hâlde init_shell() ile shell başlatılır ve döndürülür. */
static t_shell	setup_shell_and_get_input(int argc, char **argv, char **envp)
{
	t_shell	shell;

	if (argc > 1)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(argv[1], 2);
		ft_putstr_fd(": cannot execute binary file\n", 2);
		exit(126);
	}
	shell = init_shell(envp);
	return (shell);
}

/* Ana giriş noktası (entry point).
** 1. Shell başlatılır (setup_shell_and_get_input)
** 2. Sonsuz döngüde kullanıcıdan giriş okunur (get_user_input)
** 3. Giriş işlenir (handle_input):
**    - 0 → döngüden çık (exit veya EOF)
**    - 1 → bir sonraki iterasyona geç (sözdizimi hatası)
**    - 2 → komutu işle (process_command_from_input)
** 4. Döngü bittikten sonra shell temizlenir ve son exit kodu döndürülür */
int	main(int argc, char **argv, char **envp)
{
	t_shell	shell;
	char	*input;
	int		result;
	int		cmd_result;

	shell = setup_shell_and_get_input(argc, argv, envp);
	while (1)
	{
		input = get_user_input();
		result = handle_input(input, &shell);
		if (result == 0)
			break ;
		else if (result == 1)
			continue ;
		else if (result == 2)
		{
			cmd_result = process_command_from_input(input, &shell);
			if (cmd_result == 0)
				continue ;
			ft_free(input);
		}
	}
	cleanup_shell(&shell);
	return (shell.last_exit);
}
