#include "../include/minishell.h"

// Tilde expansion: ~ -> $HOME
char *ft_expand_tilde(const char *str)
{
    if (!str)
        return NULL;
    
    // ~ ile başlıyorsa
    if (str[0] == '~')
    {
        char *home = getenv("HOME");
        if (home)
        {
            // ~'ı $HOME ile değiştir
            int home_len = ft_strlen(home);
            int str_len = ft_strlen(str);
            char *new_res = ft_malloc(home_len + str_len + 1, __FILE__, __LINE__);
            
            if (new_res)
            {
                ft_strcpy(new_res, home);
                ft_strcat(new_res, str + 1); // ~ sonrası kısmı ekle
                return new_res;
            }
        }
    }
    
    // Orijinal string'i kopyala
    return ft_strdup(str);
} 