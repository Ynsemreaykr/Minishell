#include "../include/minishell.h"
#include <stdlib.h>

size_t ft_strlen(const char *s)
{
    size_t len = 0;
    while (s[len])
        len++;
    return len;
} 