#include "../include/minishell.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <readline/readline.h>
#include <fcntl.h>

// Dynamic buffer ile line-by-line okuma (rget_next_line benzeri)
char *read_line_dynamic(int fd)
{
    char *line = NULL;
    int line_size = 0;
    char buffer[1];  // Tek karakter oku
    ssize_t bytes_read;
    
    while ((bytes_read = read(fd, buffer, 1)) > 0) {
        if (buffer[0] == '\n') {
            break;  // Line sonu
        }
        
        // Line'a karakter ekle
        char *new_line = ft_malloc(line_size + 2, __FILE__, __LINE__);
        if (!new_line) {
            if (line) ft_free(line);
            return NULL;
        }
        
        if (line) {
            ft_strncpy(new_line, line, line_size);
            ft_free(line);
        } else {
            new_line[0] = '\0';
        }
        
        new_line[line_size] = buffer[0];
        new_line[line_size + 1] = '\0';
        line = new_line;
        line_size++;
    }
    
    return line;
}

static char *expand_variable_in_heredoc(const char *line, t_shell *shell)
{
    char *expanded = ft_malloc(ft_strlen(line) * 4 + 1, __FILE__, __LINE__);
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
                char *exit_str = ft_itoa(0); // Şimdilik 0
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
                        if (ft_strncmp(envp[e], var_name, strlen(var_name)) == 0 && 
                            envp[e][strlen(var_name)] == '=') 
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
                    char *exit_str = ft_itoa(0); // Şimdilik 0
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
                            if (strncmp(envp[e], var_name, strlen(var_name)) == 0 && 
                                envp[e][strlen(var_name)] == '=') 
                                {
                                var_value = envp[e] + strlen(var_name) + 1;
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

// Multiple heredoc handling - tek bir heredoc'u işler
int multiple_heredoc_input(t_heredoc *heredoc, t_shell *shell)
{
    int pipefd[2];
    if (pipe(pipefd) != 0) {
        return -1;  // Pipe oluşturulamadı
    }
    
    // Heredoc başladı - özel sinyal handler'ları kur
    setup_heredoc_signals();
    
    char *content = NULL; // heredoc'un içeriğini geçici olarak toplar
    int content_size = 0; // içeriğin toplam uzunluğunu tutar
    char *line = NULL; // readline ile okunan her satır
    int error_occurred = 0; // ctrl c / ctrl d hatalarını tutar

    char *end_delimiter; // heredoc'un bitişini belirleyen delimiter
    if (heredoc->quoted_flag) // delimiter tırnaklı ise heredoc tan çıkış için temiz halini kullan
    {
        // Quoted delimiter - cleaned version kullan
        end_delimiter = heredoc->cleaned_delimiter;
    } 
    else // delimiter tırnaksız; direk kullan
    {
        // Unquoted delimiter - original kullan
        end_delimiter = heredoc->delimiter;
    }
    
    while (1) 
    {
        // Ctrl+C kontrolü
        if (g_signal_number == SIGINT) 
        {
            error_occurred = 1;
            break;
        }
        
        line = readline("> ");
        if (!line) {
            // Ctrl+D ile çıkış durumunda
            error_occurred = 1;
            break;
        }
        

        if (ft_strcmp(line, end_delimiter) == 0) // delimiter ile karşılaştır 
        {
            ft_free(line);
            break;
        }
        
        char *content_line; // expand yapılacak veya yapılmayacak veri
        if (heredoc->quoted_flag != 0) // expand yapılmaz çünkü tırnaklı
        {
            // Quoted delimiter - expansion yapılmaz
            content_line = ft_strdup(line);
        }  
        else // expand yapılır çünkü tırnaksız
        {
            // Unquoted delimiter - expansion yapılır
            content_line = expand_variable_in_heredoc(line, shell);
        }
        
        int line_len = ft_strlen(content_line) + 1;
        char *new_content = ft_malloc(content_size + line_len + 1, __FILE__, __LINE__);
        if (content) 
        {
            ft_strcpy(new_content, content);
            ft_free(content);
        } 
        else 
        {
            new_content[0] = 0;
        }
        ft_strcat(new_content, content_line);
        ft_strcat(new_content, "\n");
        content = new_content;
        content_size += line_len;
        ft_free(content_line);
        ft_free(line);
    }
    
    if (error_occurred) 
    {
        if (content) 
            ft_free(content);
        close(pipefd[0]);
        close(pipefd[1]);
        // Heredoc bitti - hata durumu
        return -1;
    }
    
    // Content'i pipe'a yaz
    if (content) 
    {
        write(pipefd[1], content, ft_strlen(content));
        ft_free(content);
    }
    close(pipefd[1]);
    
    // Heredoc bitti - read fd döndür
    
    return pipefd[0];
}

 