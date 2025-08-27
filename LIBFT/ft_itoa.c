#include "../include/minishell.h"

static int count_digits(int n)
{
    int count = 0;
    if (n == 0)
        return 1;
    
    if (n < 0)
        count++;
    
    while (n != 0) {
        count++;
        n /= 10;
    }
    return count;
}

char *ft_itoa(int n)
{
    int digits = count_digits(n);
    char result[digits + 1];
    
    int i = digits - 1;
    
    if (n == 0) {
        result[0] = '0';
        result[1] = '\0';
        return ft_strdup(result);
    }
    
    if (n < 0) {
        result[0] = '-';
        n = -n;
    }
    
    while (n > 0) {
        result[i--] = '0' + (n % 10);
        n /= 10;
    }
    
    result[digits] = '\0';
    return ft_strdup(result);
}
