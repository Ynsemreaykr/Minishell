/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   variable_assigment.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 23:59:39 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/08/28 23:59:40 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

int	is_variable_assignment(char **argv)
{
	char	*equals;
	char	*p;

	if (!argv || !argv[0])
		return (0);
	equals = ft_strchr(argv[0], '=');
	if (!equals || equals == argv[0])
		return (0);
	p = argv[0];
	while (p < equals)
	{
		if (!ft_isalnum(*p) && *p != '_')
			return (0);
		p++;
	}
	return (1);
}
