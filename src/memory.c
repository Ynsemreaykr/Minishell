/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 17:28:01 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/08/28 18:03:43 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static t_mem_block	**get_mem_list(void)
{
	static t_mem_block	*mem_list = NULL;

	return (&mem_list);
}

void	*ft_malloc(size_t size, const char *file, int line)
{
	void		*ptr;
	t_mem_block	*block;
	size_t		file_len;

	ptr = malloc(size);
	if (!ptr)
		return (NULL);
	block = malloc(sizeof(t_mem_block));
	if (!block)
	{
		free(ptr);
		return (NULL);
	}
	block->ptr = ptr;
	block->size = size;
	file_len = ft_strlen(file);
	block->file = malloc(file_len + 1);
	if (block->file)
		ft_strcpy(block->file, file);
	block->line = line;
	block->next = *get_mem_list();
	*get_mem_list() = block;
	return (ptr);
}

static void	free_mem_block(t_mem_block **mem_list,
	t_mem_block *current, t_mem_block *prev)
{
	if (prev)
		prev->next = current->next;
	else
		*mem_list = current->next;
	free(current->file);
	free(current);
}

void	ft_free(void *ptr)
{
	t_mem_block	**mem_list;
	t_mem_block	*current;
	t_mem_block	*prev;

	if (!ptr)
		return ;
	mem_list = get_mem_list();
	current = *mem_list;
	prev = NULL;
	while (current)
	{
		if (current->ptr == ptr)
		{
			free_mem_block(mem_list, current, prev);
			free(ptr);
			return ;
		}
		prev = current;
		current = current->next;
	}
	free(ptr);
}

void	ft_mem_cleanup(void)
{
	t_mem_block	**mem_list;
	t_mem_block	*current;
	t_mem_block	*next;

	mem_list = get_mem_list();
	current = *mem_list;
	while (current)
	{
		next = current->next;
		free(current->file);
		free(current->ptr);
		free(current);
		current = next;
	}
	*mem_list = NULL;
}

void	ft_mem_print(void)
{
	t_mem_block	*current;
	int			count;
	size_t		total_size;

	current = *get_mem_list();
	count = 0;
	total_size = 0;
	printf("=== Memory Status ===\n");
	while (current)
	{
		printf("[%d] %p (%zu bytes) at %s:%d\n",
			count++, current->ptr, current->size, current->file, current->line);
		total_size += current->size;
		current = current->next;
	}
	printf("Total: %d blocks, %zu bytes\n", count, total_size);
	printf("===================\n");
}
