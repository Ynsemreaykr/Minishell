#include "../include/minishell.h"

// Sayıyı string'e çevir
static void int_to_str(int num, char *str)
{
    int i = 0;
    int temp = num;
    
    // Sayının basamak sayısını hesapla
    int digits = 0;
    if (temp == 0) {
        digits = 1;
    } else {
        while (temp != 0) {
            temp /= 10;
            digits++;
        }
    }
    
    // Sayıyı string'e çevir
    str[digits] = '\0';
    for (i = digits - 1; i >= 0; i--) {
        str[i] = '0' + (num % 10);
        num /= 10;
    }
}

void expand_env_vars(const char *src, char *dst, int dstsize, char **envp, int last_exit)
{
    int b = 0;
    int dlen = ft_strlen(src);
    dst[0] = '\0'; // Initialize destination
    
    for (int a = 0; a < dlen && b < dstsize - 1;) {
        if (src[a] == '$') {
            int var_start = a + 1;
            int var_end = var_start;
            
            // Special case for $?
            if (src[var_start] == '?') {
                char numbuf[16];
                int_to_str(last_exit, numbuf);
                int len = ft_strlen(numbuf);
                if (b + len < dstsize - 1) {
                    ft_strcpy(dst + b, numbuf);
                    b += len;
                }
                a += 2; // Skip $ and ?
                continue;
            }
            
            // Extract regular variable name
            while (src[var_end] && (ft_isalnum(src[var_end]) || src[var_end] == '_'))
                var_end++;
            
            if (var_end > var_start) {
                char varname[128] = {0};
                ft_strncpy(varname, src + var_start, var_end - var_start);
                
                int found = 0;
                for (int e = 0; envp && envp[e]; e++) {
                    if (!ft_strncmp(envp[e], varname, ft_strlen(varname)) && envp[e][ft_strlen(varname)] == '=') {
                        char *value = envp[e] + ft_strlen(varname) + 1;
                        int len = ft_strlen(value);
                        if (b + len < dstsize - 1) {
                            ft_strcpy(dst + b, value);
                            b += len;
                        }
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    // Değişken yoksa boş ekle (hiçbir şey yapmayız)
                }
                a = var_end;
            } else {
                // $ sonrasında geçerli variable name yok, literal $ olarak ekle
                if (b < dstsize - 1) {
                    dst[b++] = src[a++];
                } else {
                    break;
                }
            }
        } else {
            if (b < dstsize - 1) {
                dst[b++] = src[a++];
            } else {
                break;
            }
        }
    }
    dst[b] = '\0';
} 