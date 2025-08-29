
#include "../include/minishell.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <readline/readline.h>
#include <fcntl.h>




char *expand_variable_in_heredoc(const char *line, t_shell *shell)
{
    // Önce gerekli buffer boyutunu hesapla
    int required_size = calculate_expansion_size(line, shell);
    char *expanded = ft_malloc(required_size + 1, __FILE__, __LINE__);
    int exp_pos = 0;
    int i = 0;
    int in_quote = 0;
    char quote_char = 0;
    
    while (line[i]) 
    {
        if ((line[i] == '"' || line[i] == '\'') && !in_quote) 
        {
            // Quote başlangıcı
            quote_char = line[i];
            in_quote = 1;
            expanded[exp_pos++] = line[i++];
        } 
        else if (line[i] == quote_char && in_quote) 
        {
            // Quote bitişi
            in_quote = 0;
            quote_char = 0;
            expanded[exp_pos++] = line[i++];
        } 
        else if (line[i] == '$' && !in_quote) 
        {
            // Quote dışında $ - normal expansion
            i++; // Skip $
            if (line[i] == '?') 
            {
                // $? special case
                char *exit_str = ft_itoa(shell->last_exit);
                if (exit_str) 
                {
                    ft_strcpy(expanded + exp_pos, exit_str);    
                    exp_pos += ft_strlen(exit_str);
                    ft_free(exit_str);
                }
                i++;
            } 
            else if (ft_isalnum(line[i]) || line[i] == '_') 
            {
                // Variable name
                int var_start = i;
                while (line[i] && (ft_isalnum(line[i]) || line[i] == '_'))
                    i++;
                
                int var_len = i - var_start;
                if (var_len > 0) 
                {
                    char *var_name = ft_malloc(var_len + 1, __FILE__, __LINE__);
                    ft_strncpy(var_name, line + var_start, var_len);
                    var_name[var_len] = '\0';
                    
                    // Get environment variable
                    char **envp = get_env(shell);
                    char *var_value = NULL;
                    for (int e = 0; envp && envp[e]; e++) 
                    {
                        if (ft_strncmp(envp[e], var_name, ft_strlen(var_name)) == 0 && 
                            envp[e][ft_strlen(var_name)] == '=') 
                            {
                            var_value = envp[e] + ft_strlen(var_name) + 1;
                            break;
                        }
                    }
                    
                    // Add variable value or empty if not found
                    if (var_value) 
                    {
                        ft_strcpy(expanded + exp_pos, var_value);
                        exp_pos += ft_strlen(var_value);
                    }
                    
                    ft_free(var_name);
                }
            } 
            else 
            {
                // Just $ - literal olarak ekle
                expanded[exp_pos++] = '$';
            }
        } 


        else if (line[i] == '$' && in_quote) 
        {
            // Quote içinde $ - özel kurallar
            if (quote_char == '"')
            {
                // Double quote içinde
                i++; // Skip $
                if (line[i] == '?') 
                {
                    // $? special case
                    char *exit_str = ft_itoa(shell->last_exit);
                    if (exit_str) 
                    {
                        ft_strcpy(expanded + exp_pos, exit_str);
                        exp_pos += ft_strlen(exit_str);
                        ft_free(exit_str);
                    }
                    i++;
                } 
                else if (ft_isalnum(line[i]) || line[i] == '_') 
                {
                    // Variable name
                    int var_start = i;
                    while (line[i] && (ft_isalnum(line[i]) || line[i] == '_'))
                        i++;
                    
                    int var_len = i - var_start;
                    if (var_len > 0) 
                    {
                        char *var_name = ft_malloc(var_len + 1, __FILE__, __LINE__);
                        ft_strncpy(var_name, line + var_start, var_len);
                        var_name[var_len] = '\0';
                        
                        // Get environment variable
                        char **envp = get_env(shell);
                        char *var_value = NULL;
                        for (int e = 0; envp && envp[e]; e++) 
                        {
                                                    if (ft_strncmp(envp[e], var_name, ft_strlen(var_name)) == 0 && 
                            envp[e][ft_strlen(var_name)] == '=') 
                            {
                            var_value = envp[e] + ft_strlen(var_name) + 1;
                            break;
                        }
                        }
                        
                        // Add variable value or empty if not found
                        if (var_value) 
                        {
                            ft_strcpy(expanded + exp_pos, var_value);
                            exp_pos += ft_strlen(var_value);
                        }
                        
                        ft_free(var_name);
                    }
                } 
                else 
                {
                    // Sadece $ - literal olarak ekle
                    expanded[exp_pos++] = '$';
                }
            } 
            else 
            {
                // Single quote içinde - hiçbir expansion yapma
                expanded[exp_pos++] = line[i++];
            }
        } 
        else 
        {
            // Normal karakter
            expanded[exp_pos++] = line[i++];
        }
    }
    
    expanded[exp_pos] = '\0';
    return expanded;
}

