/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yayiker <yayiker@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 17:28:01 by yayiker           #+#    #+#             */
/*   Updated: 2025/09/10 10:23:08 by yayiker          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Özel bellek yönetim modülü.
** Ayrılan tüm bellek bloklarını bağlı liste (t_mem_block) ile takip eder.
** Böylece herhangi bir anda ft_mem_cleanup() ile tüm bellek kurtarılabilir.
** Bu yaklaşım, özellikle fork/exit senaryolarında bellek sızıntısını önler. */

#include "../include/minishell.h"

/* Tek örnekli (singleton) bellek listesi işaretçisini döndürür.
** static değişken sayesinde program boyunca tek bir liste tutulur.
** Tüm bellek fonksiyonları bu merkezi listeye erişir. */
static t_mem_block	**get_mem_list(void)
{
	static t_mem_block	*mem_list = NULL;

	return (&mem_list);
}

/* malloc() çağrısı yapar ve ayrılan bloğu izleme listesine ekler.
** Her ft_malloc çağrısı hem kullanıcı verisi hem de bir t_mem_block
** kontrol bloğu oluşturur. Başarısızlıkta NULL döner. */
void	*ft_malloc(size_t size)
{
	void		*ptr;
	t_mem_block	*block;

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
	block->next = *get_mem_list();
	*get_mem_list() = block;
	return (ptr);
}

/* Bellek listesinden tek bir blok düğümünü kaldırır.
** prev NULL ise listenin başı güncellenir.
** prev varsa önceki düğümün next'i güncellenir. */
static void	free_mem_block(t_mem_block **mem_list,
	t_mem_block *current, t_mem_block *prev)
{
	if (prev)
		prev->next = current->next;
	else
		*mem_list = current->next;
	free(current);
}

/* Verilen bellek adresini hem listeden hem de heap'ten serbest bırakır.
** - ptr NULL ise hemen döner (güvenli çağrı).
** - Liste taranır; eşleşen blok bulununca free_mem_block ile kaldırılır,
**   gerçek bellek adresi free() ile serbest bırakılır.
** - Liste dışında ayrılan bellek için de standart free() çağrılır. */
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

/* Tüm takip edilen bellek bloklarını tek seferde serbest bırakır.
** Hem kullanıcı verisi hem de kontrol blokları temizlenir.
** Program sonunda veya fork sonrası child işlemde çağrılır. */
void	ft_mem_cleanup(void)
{
	t_mem_block	**mem_list;
	t_mem_block	*current;
	t_mem_block	*next;

	mem_list = get_mem_list();
	if (!mem_list)
		return ;
	current = *mem_list;
	while (current)
	{
		next = current->next;
		if (current->ptr)
			free(current->ptr);
		free(current);
		current = next;
	}
	*mem_list = NULL;
}
