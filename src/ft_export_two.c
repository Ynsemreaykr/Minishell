/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_export_two.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 02:45:12 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/09/10 10:22:34 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

static int	process_single_export_arg(char *arg, t_shell *shell)
{
	char	*var_name;
	char	*eq;

	var_name = extract_var_name(arg);
	eq = ft_strchr(arg, '=');
	if (!is_valid_identifier(var_name))
	{
		ft_putstr_fd("minishell: export: `", 2);
		ft_putstr_fd(arg, 2);
		ft_putstr_fd("': not a valid identifier\n", 2);
		ft_free(var_name);
		return (1);
	}
	if (eq)
	{
		*eq = '\0';
		set_env_var(var_name, eq + 1, shell);
		*eq = '=';
	}
	else if (!get_env_var(var_name, shell))
		set_env_var(var_name, NULL, shell);
	ft_free(var_name);
	return (0);
}

int	ft_export(char **argv, t_shell *shell)
{
	int	i;
	int	res;

	i = 1;
	if (!argv[i])
		return (handle_no_args_export(shell));
	res = 0;
	while (argv[i])
	{
		if (process_single_export_arg(argv[i], shell) != 0)
			res = 1;
		i++;
	}
	return (res);
}
