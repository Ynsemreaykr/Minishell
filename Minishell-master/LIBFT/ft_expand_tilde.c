#include "../include/minishell.h"

char *ft_expand_tilde(const char *str)
{
    if (!str || str[0] != '~')
        return ft_strdup(str);
    
    // ~ ile başlıyorsa
    if (str[1] == '\0' || str[1] == '/') {
        // ~ veya ~/ formatı
        const char *home = getenv("HOME");
        if (!home) {
            ft_putstr_fd("cd: HOME not set\n", 2);
            return NULL;
        }
        
        if (str[1] == '\0') {
            // Sadece ~
            return ft_strdup(home);
        } else {
            // ~/path formatı
            char *result = malloc(ft_strlen(home) + ft_strlen(str));
            ft_strcpy(result, home);
            ft_strcat(result, str + 1); // +1 to skip ~
            return result;
        }
    }
    
    // ~username formatı (şimdilik desteklenmiyor, sadece ~ döndür)
    return ft_strdup(str);
} 