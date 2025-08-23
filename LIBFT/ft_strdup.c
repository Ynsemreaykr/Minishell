#include "../include/minishell.h"
#include <stdlib.h>

char *ft_strdup(const char *s)
{
    if (!s)
        return NULL;
    char *dup = ft_malloc(ft_strlen(s) + 1, __FILE__, __LINE__);
    if (!dup)
        return NULL;
    ft_strcpy(dup, s);
    return dup;
} 