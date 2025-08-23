#include "../include/minishell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct s_mem_block {
    void *ptr;
    size_t size;
    char *file;
    int line;
    struct s_mem_block *next;
} t_mem_block;

static t_mem_block **get_mem_list(void)
{
    static t_mem_block *mem_list = NULL;
    return &mem_list;
}

void *ft_malloc(size_t size, const char *file, int line)
{
    void *ptr = malloc(size);
    if (!ptr) 
        return NULL;
    
    t_mem_block *block = malloc(sizeof(t_mem_block));
    if (!block) {
        free(ptr);
        return NULL;
    }
    
    block->ptr = ptr;
    block->size = size;
    // file string'ini manuel olarak kopyala
    size_t file_len = ft_strlen(file);
    block->file = malloc(file_len + 1);
    if (block->file) {
        ft_strcpy(block->file, file);
    }
    block->line = line;
    block->next = *get_mem_list();
    *get_mem_list() = block;
    
    return ptr;
}

void ft_free(void *ptr)
{
    if (!ptr) return;
    
    t_mem_block **mem_list = get_mem_list();
    t_mem_block *current = *mem_list;
    t_mem_block *prev = NULL;
    
    while (current) {
        if (current->ptr == ptr) {
            if (prev)
                prev->next = current->next;
            else
                *mem_list = current->next;
            
            free(current->file);
            free(current);
            free(ptr);
            return;
        }
        prev = current;
        current = current->next;
    }
    
    // Eğer listede bulunamazsa normal free yap
    free(ptr);
}

void ft_mem_cleanup(void)
{
    t_mem_block **mem_list = get_mem_list();
    t_mem_block *current = *mem_list;
    t_mem_block *next;
    
    while (current) {
        next = current->next;
        // Bellek sızıntısı varsa yazdır (debug için)
        // printf("LEAK: %p (%zu bytes) allocated at %s:%d\n", 
        //        current->ptr, current->size, current->file, current->line);
        // Gerçek temizlik yap
        free(current->file);
        free(current->ptr);
        free(current);
        current = next;
    }
    *mem_list = NULL;
}

void ft_mem_print(void)
{
    t_mem_block *current = *get_mem_list();
    int count = 0;
    size_t total_size = 0;
    
    printf("=== Memory Status ===\n");
    while (current) {
        printf("[%d] %p (%zu bytes) at %s:%d\n", 
               count++, current->ptr, current->size, current->file, current->line);
        total_size += current->size;
        current = current->next;
    }
    printf("Total: %d blocks, %zu bytes\n", count, total_size);
    printf("===================\n");
} 