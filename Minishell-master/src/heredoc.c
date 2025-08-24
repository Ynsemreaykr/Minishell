#include "../include/minishell.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <readline/readline.h>
#include <fcntl.h>

// Debug fonksiyonu
void debug_heredoc(const char *message) {
    fprintf(stderr, "[HEREDOC DEBUG] %s\n", message);
    fflush(stderr);
}

void debug_heredoc_detail(const char *message, int value) {
    fprintf(stderr, "[HEREDOC DEBUG] %s: %d\n", message, value);
    fflush(stderr);
}

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
/*
int multiple_heredoc_input(t_heredoc *heredoc, t_shell *shell)
{
    debug_heredoc("Starting multiple_heredoc_input");
    
    int pipefd[2];
    if (pipe(pipefd) != 0) {
        debug_heredoc("Pipe creation failed");
        return -1;  // Pipe oluşturulamadı
    }
    
    // Heredoc başladı - özel sinyal handler'ları kur
    setup_heredoc_signals();
    debug_heredoc("Heredoc signals set up");
    
    char *content = NULL; // heredoc'un içeriğini geçici olarak toplar
    int content_size = 0; // içeriğin toplam uzunluğunu tutar
    char *line = NULL; // readline ile okunan her satır
    int error_occurred = 0; // ctrl c / ctrl d hatalarını tutar
    int input_count = 0; // Girilen satır sayısı

    char *end_delimiter; // heredoc'un bitişini belirleyen delimiter
    if (heredoc->quoted_flag) // delimiter tırnaklı ise heredoc tan çıkış için temiz halini kullan
    {
        end_delimiter = heredoc->cleaned_delimiter;
        debug_heredoc_detail("Using quoted delimiter", heredoc->quoted_flag);
    } 
    else // delimiter tırnaksız; direk kullan
    {
        end_delimiter = heredoc->delimiter;
        debug_heredoc_detail("Using unquoted delimiter", heredoc->quoted_flag);
    }
    
    while (1) 
    {
        // Ctrl+C kontrolü - hemen çık
        if (g_signal_number == SIGINT) 
        {
            debug_heredoc("SIGINT received in heredoc");
            error_occurred = 1;
            break;
        }
        
        line = readline("> ");
        
        // Ctrl+C kontrolü - readline sonrası
        if (g_signal_number == SIGINT) 
        {
            debug_heredoc("SIGINT received after readline");
            if (line) 
                ft_free(line);
            error_occurred = 1;
            break;
        }
        
        // Ctrl+D kontrolü
        if (!line) {
            debug_heredoc("Ctrl+D received in heredoc");
            // İlk Ctrl+D ise devam et, ikinci Ctrl+D ise çık
            if (input_count == 0) {
                error_occurred = 1;
                break;
            }
            break;
        }
        
        // Eğer delimiter ile eşleşirse çık
        if (ft_strcmp(line, end_delimiter) == 0)
        {
            debug_heredoc("Delimiter matched, exiting heredoc");
            ft_free(line);
            break;
        }
        
        char *content_line; // expand yapılacak veya yapılmayacak veri
        if (heredoc->quoted_flag != 0) // expand yapılmaz çünkü tırnaklı
        {
            content_line = ft_strdup(line);
            debug_heredoc("Using quoted delimiter - no expansion");
        }  
        else // expand yapılır çünkü tırnaksız
        {
            content_line = expand_variable_in_heredoc(line, shell);
            debug_heredoc("Using unquoted delimiter - expansion applied");
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
        
        input_count++; // Girilen satır sayısını artır
    }
    
    // Sinyalleri normal duruma geri al
    setup_normal_signals();
    reset_signal_state(); // Sinyal durumunu kesin olarak sıfırla
    
    if (error_occurred) 
    {
        if (content) 
            ft_free(content);
        close(pipefd[0]);
        close(pipefd[1]);
        return -1;  // Heredoc hata ile sonlandı
    }
    
    // Content'i pipe'a yaz
    if (content) 
    {
        write(pipefd[1], content, ft_strlen(content));
        ft_free(content);
    }
    close(pipefd[1]);
    
    return pipefd[0];
}

int multiple_heredoc_input(t_heredoc *heredoc, t_shell *shell)
{
    debug_heredoc("Starting multiple_heredoc_input");
    
    int pipefd[2];
    if (pipe(pipefd) != 0) {
        debug_heredoc("Pipe creation failed");
        return -1;  // Pipe oluşturulamadı
    }
    
    // Heredoc başladı - özel sinyal handler'ları kur
    setup_heredoc_signals();
    debug_heredoc("Heredoc signals set up");
    
    char *content = NULL; // heredoc'un içeriğini geçici olarak toplar
    int content_size = 0; // içeriğin toplam uzunluğunu tutar
    char *line = NULL; // readline ile okunan her satır
    int error_occurred = 0; // ctrl c / ctrl d hatalarını tutar
    int input_count = 0; // Girilen satır sayısı

    char *end_delimiter; // heredoc'un bitişini belirleyen delimiter
    if (heredoc->quoted_flag) // delimiter tırnaklı ise heredoc tan çıkış için temiz halini kullan
    {
        end_delimiter = heredoc->cleaned_delimiter;
        debug_heredoc_detail("Using quoted delimiter", heredoc->quoted_flag);
    } 
    else // delimiter tırnaksız; direk kullan
    {
        end_delimiter = heredoc->delimiter;
        debug_heredoc_detail("Using unquoted delimiter", heredoc->quoted_flag);
    }
    
    while (1) 
    {
        // Sinyal durumunu her döngü başında sıfırla
        reset_signal_state();
        
        line = readline("> ");
        
        // Ctrl+C kontrolü - readline sonrası hemen kontrol et
        if (g_signal_number == SIGINT) 
        {
            debug_heredoc("SIGINT received after readline");
            if (line) 
                ft_free(line);
            error_occurred = 1;
            break;
        }
        
        // Ctrl+D kontrolü
        if (!line) {
            debug_heredoc("Ctrl+D received in heredoc");
            error_occurred = 1;
            break;
        }
        
        // Eğer delimiter ile eşleşirse çık
        if (ft_strcmp(line, end_delimiter) == 0)
        {
            debug_heredoc("Delimiter matched, exiting heredoc");
            ft_free(line);
            break;
        }
        
        char *content_line; // expand yapılacak veya yapılmayacak veri
        if (heredoc->quoted_flag != 0) // expand yapılmaz çünkü tırnaklı
        {
            content_line = ft_strdup(line);
            debug_heredoc("Using quoted delimiter - no expansion");
        }  
        else // expand yapılır çünkü tırnaksız
        {
            content_line = expand_variable_in_heredoc(line, shell);
            debug_heredoc("Using unquoted delimiter - expansion applied");
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
        
        input_count++; // Girilen satır sayısını artır
    }
    
    // Sinyalleri normal duruma geri al
    setup_normal_signals();
    
    if (error_occurred) 
    {
        debug_heredoc("Error occurred, cleaning up and returning -1");
        if (content) 
            ft_free(content);
        close(pipefd[0]);
        close(pipefd[1]);
        
        // Shell'in exit code'unu Ctrl+C için ayarla
        shell->last_exit = 130;
        
        // Sinyal durumunu sıfırla
        reset_signal_state();
        
        return -1;  // Heredoc hata ile sonlandı
    }
    
    // Content'i pipe'a yaz
    if (content) 
    {
        write(pipefd[1], content, ft_strlen(content));
        ft_free(content);
    }
    close(pipefd[1]);
    
    // Sinyal durumunu sıfırla
    reset_signal_state();
    
    return pipefd[0];
}
int multiple_heredoc_input(t_heredoc *heredoc, t_shell *shell)
{
    debug_heredoc("Starting multiple_heredoc_input");
    
    int pipefd[2];
    if (pipe(pipefd) != 0) {
        debug_heredoc("Pipe creation failed");
        return -1;  // Pipe oluşturulamadı
    }
    
    // Heredoc başladı - özel sinyal handler'ları kur
    setup_heredoc_signals();
    debug_heredoc("Heredoc signals set up");
    
    char *content = NULL; // heredoc'un içeriğini geçici olarak toplar
    int content_size = 0; // içeriğin toplam uzunluğunu tutar
    char *line = NULL; // readline ile okunan her satır
    int error_occurred = 0; // ctrl c / ctrl d hatalarını tutar
    int input_count = 0; // Girilen satır sayısı

    char *end_delimiter; // heredoc'un bitişini belirleyen delimiter
    if (heredoc->quoted_flag) // delimiter tırnaklı ise heredoc tan çıkış için temiz halini kullan
    {
        end_delimiter = heredoc->cleaned_delimiter;
        debug_heredoc_detail("Using quoted delimiter", heredoc->quoted_flag);
    } 
    else // delimiter tırnaksız; direk kullan
    {
        end_delimiter = heredoc->delimiter;
        debug_heredoc_detail("Using unquoted delimiter", heredoc->quoted_flag);
    }
    
    while (1) 
    {        
        // READLINE ÖNCESI sinyal kontrolü
        if (g_signal_number == SIGINT) 
        {
            debug_heredoc("SIGINT detected before readline");
            error_occurred = 1;
            break;
        }
        
        line = readline("> ");
        
        // Ctrl+C kontrolü - readline sonrası hemen kontrol et
        if (g_signal_number == SIGINT) 
        {
            debug_heredoc("SIGINT received after readline");
            if (line) 
                ft_free(line);
            error_occurred = 1;
            break;
        }
        
        // Ctrl+D kontrolü
        if (!line) {
            debug_heredoc("Ctrl+D received in heredoc");
            error_occurred = 1;
            break;
        }
        
        // Eğer delimiter ile eşleşirse çık
        if (ft_strcmp(line, end_delimiter) == 0)
        {
            debug_heredoc("Delimiter matched, exiting heredoc");
            ft_free(line);
            break;
        }
        
        char *content_line; // expand yapılacak veya yapılmayacak veri
        if (heredoc->quoted_flag != 0) // expand yapılmaz çünkü tırnaklı
        {
            content_line = ft_strdup(line);
            debug_heredoc("Using quoted delimiter - no expansion");
        }  
        else // expand yapılır çünkü tırnaksız
        {
            content_line = expand_variable_in_heredoc(line, shell);
            debug_heredoc("Using unquoted delimiter - expansion applied");
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
        
        input_count++; // Girilen satır sayısını artır
    }
    
    // Sinyalleri normal duruma geri al
    setup_normal_signals();
    
    if (error_occurred) 
    {
        debug_heredoc("Error occurred, cleaning up and returning -1");
        if (content) 
            ft_free(content);
        close(pipefd[0]);
        close(pipefd[1]);
        
        // Shell'in exit code'unu Ctrl+C için ayarla
        shell->last_exit = 130;
        
        // Sinyal durumunu sıfırla
        reset_signal_state();
        
        return -1;  // Heredoc hata ile sonlandı
    }
    
    // Content'i pipe'a yaz
    if (content) 
    {
        write(pipefd[1], content, ft_strlen(content));
        ft_free(content);
    }
    close(pipefd[1]);
    
    // Sinyal durumunu sıfırla
    reset_signal_state();
    
    return pipefd[0];
}
int multiple_heredoc_input(t_heredoc *heredoc, t_shell *shell)
{
    debug_heredoc("Starting multiple_heredoc_input");
    
    int pipefd[2];
    if (pipe(pipefd) != 0) {
        debug_heredoc("Pipe creation failed");
        return -1;  // Pipe oluşturulamadı
    }
    
    // Heredoc başladı - özel sinyal handler'ları kur
    setup_heredoc_signals();
    debug_heredoc("Heredoc signals set up");
    
    char *content = NULL; // heredoc'un içeriğini geçici olarak toplar
    int content_size = 0; // içeriğin toplam uzunluğunu tutar
    char *line = NULL; // readline ile okunan her satır
    int error_occurred = 0; // ctrl c / ctrl d hatalarını tutar
    int input_count = 0; // Girilen satır sayısı

    char *end_delimiter; // heredoc'un bitişini belirleyen delimiter
    if (heredoc->quoted_flag) // delimiter tırnaklı ise heredoc tan çıkış için temiz halini kullan
    {
        end_delimiter = heredoc->cleaned_delimiter;
        debug_heredoc_detail("Using quoted delimiter", heredoc->quoted_flag);
    } 
    else // delimiter tırnaksız; direk kullan
    {
        end_delimiter = heredoc->delimiter;
        debug_heredoc_detail("Using unquoted delimiter", heredoc->quoted_flag);
    }
    
    while (1) 
    {        
        // READLINE ÖNCESI sinyal kontrolü
        if (g_signal_number == SIGINT) 
        {
            debug_heredoc("SIGINT detected before readline");
            error_occurred = 1;
            break;
        }
        
        line = readline("> ");
        
        // Ctrl+C kontrolü - readline sonrası VE readline NULL döndüyse
        if (g_signal_number == SIGINT || (line == NULL && g_signal_number == SIGINT)) 
        {
            debug_heredoc("SIGINT received after readline");
            if (line) 
                ft_free(line);
            error_occurred = 1;
            break;
        }
        
        // Ctrl+D kontrolü
        if (!line) {
            debug_heredoc("Ctrl+D received in heredoc");
            error_occurred = 1;
            break;
        }
        
        // Eğer delimiter ile eşleşirse çık
        if (ft_strcmp(line, end_delimiter) == 0)
        {
            debug_heredoc("Delimiter matched, exiting heredoc");
            ft_free(line);
            break;
        }
        
        char *content_line; // expand yapılacak veya yapılmayacak veri
        if (heredoc->quoted_flag != 0) // expand yapılmaz çünkü tırnaklı
        {
            content_line = ft_strdup(line);
            debug_heredoc("Using quoted delimiter - no expansion");
        }  
        else // expand yapılır çünkü tırnaksız
        {
            content_line = expand_variable_in_heredoc(line, shell);
            debug_heredoc("Using unquoted delimiter - expansion applied");
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
        
        input_count++; // Girilen satır sayısını artır
    }
    
    // Sinyalleri normal duruma geri al
    setup_normal_signals();
    
    if (error_occurred) 
    {
        debug_heredoc("Error occurred, cleaning up and returning -1");
        if (content) 
            ft_free(content);
        close(pipefd[0]);
        close(pipefd[1]);
        
        // Shell'in exit code'unu Ctrl+C için ayarla
        shell->last_exit = 130;
        
        // Sinyal durumunu sıfırla
        reset_signal_state();
        
        return -1;  // Heredoc hata ile sonlandı
    }
    
    // Content'i pipe'a yaz
    if (content) 
    {
        write(pipefd[1], content, ft_strlen(content));
        ft_free(content);
    }
    close(pipefd[1]);
    
    // Sinyal durumunu sıfırla
    reset_signal_state();
    
    return pipefd[0];
}
    
    int multiple_heredoc_input(t_heredoc *heredoc, t_shell *shell)
{
    debug_heredoc("Starting multiple_heredoc_input");
    
    int pipefd[2];
    if (pipe(pipefd) != 0) {
        debug_heredoc("Pipe creation failed");
        return -1;  // Pipe oluşturulamadı
    }
    
    // Heredoc başladı - özel sinyal handler'ları kur
    setup_heredoc_signals();
    debug_heredoc("Heredoc signals set up");
    
    char *content = NULL; // heredoc'un içeriğini geçici olarak toplar
    int content_size = 0; // içeriğin toplam uzunluğunu tutar
    char *line = NULL; // readline ile okunan her satır
    int error_occurred = 0; // ctrl c / ctrl d hatalarını tutar
    int input_count = 0; // Girilen satır sayısı

    char *end_delimiter; // heredoc'un bitişini belirleyen delimiter
    if (heredoc->quoted_flag) // delimiter tırnaklı ise heredoc tan çıkış için temiz halini kullan
    {
        end_delimiter = heredoc->cleaned_delimiter;
        debug_heredoc_detail("Using quoted delimiter", heredoc->quoted_flag);
    } 
    else // delimiter tırnaksız; direk kullan
    {
        end_delimiter = heredoc->delimiter;
        debug_heredoc_detail("Using unquoted delimiter", heredoc->quoted_flag);
    }
    
    while (1) 
    {        
        // Sinyal kontrolü - readline ÖNCESI
        if (g_signal_number == SIGINT) 
        {
            debug_heredoc("SIGINT detected before readline - breaking");
            error_occurred = 1;
            break;
        }
        
        line = readline("> ");
        
        // Ctrl+C kontrolü - signal yakalandı mı?
        if (g_signal_number == SIGINT) 
        {
            debug_heredoc("SIGINT received - breaking from loop");
            if (line) 
                ft_free(line);
            error_occurred = 1;
            break;
        }
        
        // Ctrl+D kontrolü (line NULL ise)
        if (!line) {
            debug_heredoc("Ctrl+D received in heredoc");
            error_occurred = 1;
            break;
        }
        
        // Eğer delimiter ile eşleşirse çık
        if (ft_strcmp(line, end_delimiter) == 0)
        {
            debug_heredoc("Delimiter matched, exiting heredoc");
            ft_free(line);
            break;
        }
        
        char *content_line; // expand yapılacak veya yapılmayacak veri
        if (heredoc->quoted_flag != 0) // expand yapılmaz çünkü tırnaklı
        {
            content_line = ft_strdup(line);
            debug_heredoc("Using quoted delimiter - no expansion");
        }  
        else // expand yapılır çünkü tırnaksız
        {
            content_line = expand_variable_in_heredoc(line, shell);
            debug_heredoc("Using unquoted delimiter - expansion applied");
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
        
        input_count++; // Girilen satır sayısını artır
    }
    
    // Sinyalleri normal duruma geri al
    setup_normal_signals();
    
    if (error_occurred) 
    {
        debug_heredoc("Error occurred, cleaning up and returning -1");
        if (content) 
            ft_free(content);
        close(pipefd[0]);
        close(pipefd[1]);
        
        // Shell'in exit code'unu Ctrl+C için ayarla
        shell->last_exit = 130;
        
        // Sinyal durumunu sıfırla
        reset_signal_state();
        
        return -1;  // Heredoc hata ile sonlandı
    }
    
    // Content'i pipe'a yaz
    if (content) 
    {
        write(pipefd[1], content, ft_strlen(content));
        ft_free(content);
    }
    close(pipefd[1]);
    
    // Sinyal durumunu sıfırla
    reset_signal_state();
    
    return pipefd[0];
}
    

int multiple_heredoc_input(t_heredoc *heredoc, t_shell *shell)
{
    debug_heredoc("Starting multiple_heredoc_input");
    
    int pipefd[2];
    if (pipe(pipefd) != 0) {
        debug_heredoc("Pipe creation failed");
        return -1;
    }
    
    // ÖNCEDEN sinyal durumunu temizle
    g_signal_number = 0;
    
    // Heredoc sinyallerini kur
    setup_heredoc_signals();
    debug_heredoc("Heredoc signals set up");
    
    char *content = NULL;
    int content_size = 0;
    char *line = NULL;
    int error_occurred = 0;

    char *end_delimiter;
    if (heredoc->quoted_flag) {
        end_delimiter = heredoc->cleaned_delimiter;
        debug_heredoc_detail("Using quoted delimiter", heredoc->quoted_flag);
    } else {
        end_delimiter = heredoc->delimiter;
        debug_heredoc_detail("Using unquoted delimiter", heredoc->quoted_flag);
    }
    
    while (1) 
    {        
        line = readline("> ");
        
        // İLK ÖNCE: readline NULL döndü mü? (Ctrl+C veya Ctrl+D)
        if (!line) {
            if (g_signal_number == SIGINT) {
                debug_heredoc("SIGINT received - readline returned NULL");
                error_occurred = 1;
            } else {
                debug_heredoc("Ctrl+D received in heredoc");
                error_occurred = 1;
            }
            break;
        }
        
        // İKİNCİ: Normal signal kontrolü
        if (g_signal_number == SIGINT) {
            debug_heredoc("SIGINT received - breaking from loop");
            ft_free(line);
            error_occurred = 1;
            break;
        }
        
        // Delimiter kontrolü
        if (ft_strcmp(line, end_delimiter) == 0) {
            debug_heredoc("Delimiter matched, exiting heredoc");
            ft_free(line);
            break;
        }
        
        // Content işleme
        char *content_line;
        if (heredoc->quoted_flag != 0) {
            content_line = ft_strdup(line);
            debug_heredoc("Using quoted delimiter - no expansion");
        } else {
            content_line = expand_variable_in_heredoc(line, shell);
            debug_heredoc("Using unquoted delimiter - expansion applied");
        }
        
        // Content'e ekle
        int line_len = ft_strlen(content_line) + 1;
        char *new_content = ft_malloc(content_size + line_len + 1, __FILE__, __LINE__);
        if (content) {
            ft_strcpy(new_content, content);
            ft_free(content);
        } else {
            new_content[0] = 0;
        }
        ft_strcat(new_content, content_line);
        ft_strcat(new_content, "\n");
        content = new_content;
        content_size += line_len;
        
        ft_free(content_line);
        ft_free(line);
    }
    
    // Normal sinyalleri geri kur
    setup_normal_signals();
    
    if (error_occurred) {
        debug_heredoc("Error occurred, cleaning up and returning -1");
        if (content) 
            ft_free(content);
        close(pipefd[0]);
        close(pipefd[1]);
        
        // Shell'in exit code'unu ayarla
        if (g_signal_number == SIGINT) {
            shell->last_exit = 130;
        }
        
        // Sinyal durumunu temizle
        g_signal_number = 0;
        
        return -1;
    }
    
    // Content'i pipe'a yaz
    if (content) {
        write(pipefd[1], content, ft_strlen(content));
        ft_free(content);
    }
    close(pipefd[1]);
    
    // Sinyal durumunu temizle
    g_signal_number = 0;
    
    return pipefd[0];
}*/

int multiple_heredoc_input(t_heredoc *heredoc, t_shell *shell)
{
    debug_heredoc("Starting multiple_heredoc_input");
    
    int pipefd[2];
    if (pipe(pipefd) != 0) {
        debug_heredoc("Pipe creation failed");
        return -1;
    }
    
    // ÖNCEDEN sinyal durumunu temizle
    g_signal_number = 0;
    
    // Heredoc sinyallerini kur
    setup_heredoc_signals();
    debug_heredoc("Heredoc signals set up");
    
    char *content = NULL;
    int content_size = 0;
    char *line = NULL;
    int error_occurred = 0;

    char *end_delimiter;
    if (heredoc->quoted_flag) {
        end_delimiter = heredoc->cleaned_delimiter;
        debug_heredoc_detail("Using quoted delimiter", heredoc->quoted_flag);
    } else {
        end_delimiter = heredoc->delimiter;
        debug_heredoc_detail("Using unquoted delimiter", heredoc->quoted_flag);
    }
    
    while (1) 
    {      
        /*
        // Signal check BEFORE readline
        if (g_signal_number == SIGINT) {
            debug_heredoc("SIGINT detected before readline");
            error_occurred = 1;
            break;
        }
        
        line = readline("> ");
        
        // CRITICAL: Check BOTH conditions
        if (!line || g_signal_number == SIGINT) {
            if (g_signal_number == SIGINT) {
                debug_heredoc("SIGINT received - breaking from heredoc");
                error_occurred = 1;
            } else {
                debug_heredoc("Ctrl+D received in heredoc");
                error_occurred = 1;
            }
            if (line) ft_free(line);
            break;
        }
        */
        if (g_signal_number == SIGINT) {
            debug_heredoc("SIGINT detected before readline");
            error_occurred = 1;
            break;
        }
        
        // Timeout ile readline kullan - bu çok önemli!
        line = readline("> ");
        
        // Readline dönüş kontrolü
        if (!line) {
            if (g_signal_number == SIGINT) {
                debug_heredoc("SIGINT received - readline returned NULL");
            } else {
                debug_heredoc("Ctrl+D received in heredoc");
            }
            error_occurred = 1;
            break;
        }
        
        // Signal kontrolü readline SONRASI
        if (g_signal_number == SIGINT) {
            debug_heredoc("SIGINT received - breaking from heredoc");
            ft_free(line);
            error_occurred = 1;
            break;
        }

        // Delimiter kontrolü
        if (ft_strcmp(line, end_delimiter) == 0) {
            debug_heredoc("Delimiter matched, exiting heredoc");
            ft_free(line);
            break;
        }
        
        // Content işleme
        char *content_line;
        if (heredoc->quoted_flag != 0) {
            content_line = ft_strdup(line);
            debug_heredoc("Using quoted delimiter - no expansion");
        } else {
            content_line = expand_variable_in_heredoc(line, shell);
            debug_heredoc("Using unquoted delimiter - expansion applied");
        }
        
        // Content'e ekle
        int line_len = ft_strlen(content_line) + 1;
        char *new_content = ft_malloc(content_size + line_len + 1, __FILE__, __LINE__);
        if (content) {
            ft_strcpy(new_content, content);
            ft_free(content);
        } else {
            new_content[0] = 0;
        }
        ft_strcat(new_content, content_line);
        ft_strcat(new_content, "\n");
        content = new_content;
        content_size += line_len;
        
        ft_free(content_line);
        ft_free(line);
    }
    
    // Normal sinyalleri geri kur
    setup_normal_signals();
    
    if (error_occurred) {
        debug_heredoc("Error occurred, cleaning up and returning -1");
        if (content) 
            ft_free(content);
        close(pipefd[0]);
        close(pipefd[1]);
        
        // Shell'in exit code'unu ayarla
        if (g_signal_number == SIGINT) {
            shell->last_exit = 130;
        }
        
        // Sinyal durumunu temizle
        g_signal_number = 0;
        
        return -1;
    }
    
    // Content'i pipe'a yaz
    if (content) {
        write(pipefd[1], content, ft_strlen(content));
        ft_free(content);
    }
    close(pipefd[1]);
    
    // Sinyal durumunu temizle
    g_signal_number = 0;
    
    return pipefd[0];
}