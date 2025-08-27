#include "../include/minishell.h"
#include <stdlib.h>

static int ft_is_delimiter(char c, const char *delim)
{
    while (*delim)
    {
        if (c == *delim)
            return 1;
        delim++;
    }
    return 0;
}

char *ft_strtok(char *str, const char *delim)
{
    static char *save_ptr = NULL;
    return ft_strtok_r(str, delim, &save_ptr);
}

char *ft_strtok_r(char *str, const char *delim, char **saveptr) // delimiter yerine \0 yazar
{
    char *token_start;
    
    if (str)
        *saveptr = str; // null değilse başlangıç pointer ı al
    
    if (!*saveptr || !**saveptr)
        return NULL;
    
    // Skip leading delimiters
    while (**saveptr && ft_is_delimiter(**saveptr, delim)) // başta boşluk vya | varsa atla
        (*saveptr)++;
    
    if (!**saveptr)
        return NULL;
    
    token_start = *saveptr;
    
    // Find end of token
    while (**saveptr && !ft_is_delimiter(**saveptr, delim)) // | görülene kadar saveptr arttırılır
        (*saveptr)++;
    
    if (**saveptr)
    {
        **saveptr = '\0'; 
        (*saveptr)++;
    }
    
    return token_start;
} 