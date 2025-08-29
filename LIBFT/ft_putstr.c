#include "../include/minishell.h"
#include<unistd.h>

void ft_putstr_fd(const char *str, int fd)
{
    if (!str)
        return;
    
    while (*str)
        write(fd, str++, 1);
} 