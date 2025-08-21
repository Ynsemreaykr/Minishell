#include "../include/minishell.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <readline/readline.h>

// read() ile line-by-line okuma yapan yardımcı fonksiyon
static char *read_line_from_fd(int fd, char *buffer, int buffer_size)
{
    int i = 0;
    char c;
    
    while (i < buffer_size - 1) {
        int bytes_read = read(fd, &c, 1);
        if (bytes_read <= 0) {
            // EOF veya hata
            if (i == 0) return NULL;
            break;
        }
        if (c == '\n') {
            break;
        }
        buffer[i++] = c;
    }
    buffer[i] = '\0';
    return buffer;
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

// Multiple heredoc handling - sequential olarak tüm heredoc'ları okur, son content'i döndürür
int multiple_heredoc_input(char **delimiters, char **cleaned_delimiters, int *quoted_flags, int count, t_shell *shell)
{
    int pipefd[2];
    pipe(pipefd); // pipe ile ayrılmıi iki komut arası iletişim kurar
    char *final_content = NULL; // son herodocun içeriğini saklar
    
    // Heredoc başladı - özel sinyal handler'ları kur
    setup_heredoc_signals();
    
    // Tüm heredoc'ları sequential olarak oku
    for (int h = 0; h < count; h++)     
    {
        char *content = NULL; // o an işlenen her herodcun içeriğini geçici olarak toplar
        int content_size = 0; // içeriğin toplam uzunluğunu tutar
        char *line = NULL; // readline ile okunan her satır
        int error_occurred = 0; // ctrl c / ctrl d hatalarını tutar
    

        char *end_delimiter; // herodocun bitişini belirleyen delimiter
        if (quoted_flags[h]) // delimiter tırnaklı ise temiz halini kullan
        {
            // Quoted delimiter - cleaned version kullan
            end_delimiter = cleaned_delimiters[h];
        } 
        else // delimiter tırnaksız; direk kullan
        {
            // Unquoted delimiter - original kullan
            end_delimiter = delimiters[h];
        }
        
        while (1) 
        {
            // Ctrl+C kontrolü
            if (g_signal_number == SIGINT) 
            {
                error_occurred = 1;
                break;
            }
            
            // eğer stdin bir terminal ise readline kullan
            if (isatty(STDIN_FILENO)) // KOŞUL KALDIRILABİLİR Mİ
            {
                line = readline("> ");
                if (!line) {
                    // Ctrl+D ile çıkış durumunda
                    error_occurred = 1;
                    break;
                }
            } 
            else // KOŞUL KALDIRILABİLİR Mİ
            {
                // Non-interactive mode - stdin'den direkt oku
                char buffer[1024];
                if (!read_line_from_fd(0, buffer, sizeof(buffer))) 
                {
                    // EOF veya hata
                    error_occurred = 1;
                    break;
                }
                line = ft_strdup(buffer);
            }
            

            if (ft_strcmp(line, end_delimiter) == 0) // delimiter ile karşılaştır 
            {
                ft_free(line);
                break;
            }
            
            char *content_line; // expand yapılacak veya yapılmayacak veri
            if (quoted_flags[h] != 0) // expand yapılmaz çünkü tırnaklı
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
            if (final_content) 
                ft_free(final_content);
            close(pipefd[0]);
            close(pipefd[1]);
            // Heredoc bitti - t_heredoc->enabled ile takip ediliyor
            return -1;
        }
        
        // Son heredoc değilse content'i free le
        if (h < count - 1) 
        {
            if (content) 
                ft_free(content);
        } 
        else 
        {
            // Son heredoc - content'i final olarak sakla
            final_content = content;
        }
    }
    
    // Son content'i pipe'a yaz
    if (final_content) 
    {
        write(pipefd[1], final_content, ft_strlen(final_content));
        ft_free(final_content);
    }
    close(pipefd[1]);
    
    // Heredoc bitti - t_heredoc->enabled ile takip ediliyor
    
    return pipefd[0];
}

 