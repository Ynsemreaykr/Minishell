/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 17:15:37 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/10 10:23:03 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Ana döngü yardımcı fonksiyonları.
** Kullanıcı girdisi parse edildikten sonra komut çalıştırma
** ve özel durumların yönetimini içerir. */

#include "../include/minishell.h"

/* Komut listesini execute_command_main'e iletir.
** cmds NULL ise 1 (boş komut), aksi hâlde execute_command_main sonucu döner. */
static int	process_command(t_cmd *cmds, t_shell *shell)
{
	if (!cmds)
		return (1);
	return (execute_command_main(cmds, shell));
}

/* Boş string komut durumunu ele alır ("" gibi).
** - Yalnızca boşluktan sonra "" geliyorsa → hata 127 (komut bulunamadı)
** - Aksi hâlde last_exit = 0 atanır.
** Her iki durumda da cmds listesi serbest bırakılır. */
static int	handle_empty_string_cmd(t_cmd *cmds,
				const char *input, t_shell *shell)
{
	int	i;

	i = 0;
	while (input[i] && (input[i] == ' ' || input[i] == '\t'))
		i++;
	if (input[i] == '"' && input[i + 1] == '"')
	{
		ft_putstr_fd("command not found\n", 2);
		shell->last_exit = 127;
	}
	else
		shell->last_exit = 0;
	free_cmds(cmds);
	return (1);
}

/* Boş veya geçersiz komut listesini kontrol eder:
** - cmds NULL ise girişin tamamen boşluktan mı oluştuğunu inceler,
**   buna göre last_exit = 0 veya 2 atar ve 1 döndürür.
** - cmds->argv ve cmds->redirs ikisi de NULL ise → listesi temizle, 1 döndür.
** - argv[0] NULL ve redirs yoksa → handle_empty_string_cmd çağır.
** 0 döndürmesi → komutun işlenmeye devam etmesi gerektiğini belirtir. */
static int	handle_empty_or_invalid_cmd(t_cmd *cmds,
				const char *input, t_shell *shell)
{
	int	i;

	if (!cmds)
	{
		i = 0;
		while (input[i] && (input[i] == ' ' || input[i] == '\t'))
			i++;
		if (!input[i])
			shell->last_exit = 0;
		else
			shell->last_exit = 2;
		return (1);
	}
	if (!cmds->argv && !cmds->redirs)
	{
		free_cmds(cmds);
		return (1);
	}
	if (cmds->argv && !cmds->argv[0] && !cmds->redirs)
		return (handle_empty_string_cmd(cmds, input, shell));
	return (0);
}

/* Özel durum komutlarını kontrol eder:
** - Boş (sıfır uzunluklu) argv[0] → "command not found" + 127
** - "./minishell ./minishell" → binary çalıştırılamaz + 126
** Bu durumlardan biri oluşursa cmds serbest bırakılır ve 1 döndürülür.
** Normal durumda 0 döndürülür. */
static int	handle_special_cases(t_cmd *cmds, t_shell *shell)
{
	if (cmds->argv && cmds->argv[0] && ft_strlen(cmds->argv[0]) == 0)
	{
		ft_putstr_fd("minishell: : command not found\n", 2);
		shell->last_exit = 127;
		free_cmds(cmds);
		return (1);
	}
	if (cmds->argv && cmds->argv[0] && cmds->argv[1]
		&& !ft_strcmp(cmds->argv[0], "./minishell")
		&& !ft_strcmp(cmds->argv[1], "./minishell"))
	{
		ft_putstr_fd("./minishell: ./minishell: cannot execute ", 2);
		ft_putstr_fd("binary file\n", 2);
		shell->last_exit = 126;
		free_cmds(cmds);
		return (1);
	}
	return (0);
}

/* Ham giriş dizisinden komutu ayrıştırıp çalıştırır.
** Akış:
**   1. parse_commands() ile t_cmd bağlı listesi oluşturulur.
**   2. handle_empty_or_invalid_cmd() → boş/geçersiz komut kontrolü.
**   3. handle_special_cases() → özel durum kontrolü.
**   4. process_command() → execute_command_main() çağrısı.
**   5. cmds listesi serbest bırakılır. */
int	process_command_from_input(const char *input, t_shell *shell)
{
	t_cmd	*cmds;
	int		result;

	cmds = parse_commands(input, shell);
	if (handle_empty_or_invalid_cmd(cmds, input, shell))
		return (0);
	if (handle_special_cases(cmds, shell))
		return (0);
	result = process_command(cmds, shell);
	free_cmds(cmds);
	return (result);
}
