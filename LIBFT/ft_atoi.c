#include "../include/minishell.h"

int ft_atoi(const char *str)
{
    int result = 0;
    int sign = 1;
    int i = 0;
    
    // Skip whitespace
    while (str[i] == ' ' || str[i] == '\t' || str[i] == '\n' || 
           str[i] == '\v' || str[i] == '\f' || str[i] == '\r')
        i++;
    
    // Handle sign
    if (str[i] == '-' || str[i] == '+')
    {
        if (str[i] == '-')
            sign = -1;
        i++;
    }
    
    // Convert digits
    while (str[i] >= '0' && str[i] <= '9')
    {
        result = result * 10 + (str[i] - '0');
        i++;
    }
    
    return sign * result;
} 