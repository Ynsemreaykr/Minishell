#include "../include/minishell.h"
#include <stdlib.h>

void *ft_calloc(size_t nmemb, size_t size)
{
    void *ptr;
    size_t total_size = nmemb * size;
    
    if (nmemb && size && total_size / nmemb != size)
        return NULL; // Overflow check
    
    ptr = malloc(total_size);
    if (!ptr)
        return NULL;
    
    // Zero out the memory
    unsigned char *p = ptr;
    for (size_t i = 0; i < total_size; i++)
        p[i] = 0;
    
    return ptr;
} 