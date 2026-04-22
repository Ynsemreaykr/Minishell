/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_path.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 17:36:54 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/10 10:23:15 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Ortam değişkenleri üzerinden PATH'in bulunması ve içindeki her bir yolda
** komut isminin aranmasını sağlayan modül. */

#include "../include/minishell.h"
#include <unistd.h>

/* envp dizisinde "PATH=" önekiyle başlayan değeri bulur,
** ':' karakterinden bölerek ft_split yardımıyla klasörleri diziye atar.
** PATH yoksa (unset edildiyse vs) NULL döner. */
char	**parse_path(char **envp)
{
	int		i;
	char	*env_path;

	i = 0;
	env_path = NULL;
	while (envp[i])
	{
		if (!ft_strncmp(envp[i], "PATH=", 5))
		{
			env_path = envp[i] + 5;
			break ;
		}
		i++;
	}
	if (!env_path)
		return (NULL);
	return (ft_split(env_path, ':'));
}

/* parse_path ile ayrılan tüm klasör yollarının sonuna "/" ve aranılan komutun
** kendisini strjoin ile birleştire birleştire ilerler (Örn /bin + / + ls).
** Erişilebilir bir dosya bulunduğunda erişim kodunu geri yollar (0 = bulundu).
** Tüm klasörler tükendiği halde çalıştırılabilir izne sahip bir komut denk gelmezse
** -1 döner (Command not found). */
int	search_in_path(char *command, char **splitted_path, char **full_path)
{
	int		i;
	char	*temp_path;

	if (!splitted_path)
		return (-1);
	i = 0;
	while (splitted_path[i])
	{
		temp_path = ft_strjoin(splitted_path[i], "/");
		*full_path = ft_strjoin(temp_path, command);
		ft_free(temp_path);
		if (access(*full_path, X_OK) == 0)
			return (0);
		ft_free(*full_path);
		i++;
	}
	*full_path = NULL;
	return (-1);
}
