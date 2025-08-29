#include "../include/minishell.h"

char *ft_strjoin(const char *s1, const char *s2)
{
    char *result;
    int len1, len2;
    
    if (!s1 && !s2)
        return ft_strdup("");
    if (!s1)
        return ft_strdup(s2);
    if (!s2)
        return ft_strdup(s1);
    
    len1 = ft_strlen(s1);
    len2 = ft_strlen(s2);
    
    result = ft_malloc(len1 + len2 + 1, __FILE__, __LINE__);
    if (!result)
        return NULL;
    
    ft_strcpy(result, s1);
    ft_strcat(result, s2);
    
    return result;
} 