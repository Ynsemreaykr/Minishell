#include "../include/minishell.h"

void ft_putstr(const char *str)
{
    if (!str)
        return;
    
    while (*str)
        write(1, str++, 1);
}

void ft_putstr_fd(const char *str, int fd)
{
    if (!str)
        return;
    
    while (*str)
        write(fd, str++, 1);
} 