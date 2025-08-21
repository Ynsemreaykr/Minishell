#include "../include/minishell.h"
#include <stdlib.h>

char *ft_strdup(const char *s)
{
    if (!s)
        return NULL;
    char *dup = malloc(ft_strlen(s) + 1);
    if (!dup)
        return NULL;
    ft_strcpy(dup, s);
    return dup;
} 