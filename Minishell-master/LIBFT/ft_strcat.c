#include "../include/minishell.h"

char *ft_strcat(char *dest, const char *src)
{
    char *ptr = dest + ft_strlen(dest);
    while (*src)
    {
        *ptr = *src;
        ptr++;
        src++;
    }
    *ptr = '\0';
    return dest;
} 