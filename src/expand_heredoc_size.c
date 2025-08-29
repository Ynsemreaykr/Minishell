
#include "../include/minishell.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <readline/readline.h>
#include <fcntl.h>




int calculate_expansion_size(const char *line, t_shell *shell)
{
    int size = 0;
    int i = 0;
    
    while (line[i]) 
    {
        if (line[i] == '$') 
        {
            i++;
            if (line[i] == '?') 
            {
                char *exit_str = ft_itoa(shell->last_exit);
                if (exit_str) 
                {
                    size += ft_strlen(exit_str);
                    ft_free(exit_str);
                }
                i++;
            } 
            else if (ft_isalnum(line[i]) || line[i] == '_') 
            {
                int var_start = i;
                while (line[i] && (ft_isalnum(line[i]) || line[i] == '_'))
                    i++;
                
                int var_len = i - var_start;
                if (var_len > 0) 
                {
                    char *var_name = ft_malloc(var_len + 1, __FILE__, __LINE__);
                    ft_strncpy(var_name, line + var_start, var_len);
                    var_name[var_len] = '\0';
                    
                    // Environment variable değerinin boyutunu hesapla
                    char **envp = get_env(shell);
                    for (int e = 0; envp && envp[e]; e++) 
                    {
                        if (ft_strncmp(envp[e], var_name, ft_strlen(var_name)) == 0 && 
                            envp[e][ft_strlen(var_name)] == '=') 
                        {
                            size += ft_strlen(envp[e] + ft_strlen(var_name) + 1);
                            break;
                        }
                    }
                    
                    ft_free(var_name);
                }
            } 
            else 
            {
                // Just $ - literal
                size++;
            }
        } 
        else 
        {
            // Normal karakter
            size++;
            i++;
        }
    }
    
    return size;
}
