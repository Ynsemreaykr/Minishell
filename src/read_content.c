
#include "../include/minishell.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <readline/readline.h>
#include <fcntl.h>






// Heredoc content'ini oku ve string olarak döndür
char *read_heredoc_content(int fd)
{
    char *content = NULL;
    int content_size = 0;
    char *line;
    
    while ((line = read_line_dynamic(fd)) != NULL) {
        int line_len = ft_strlen(line) + 1;  // +1 for newline
        char *new_content = ft_malloc(content_size + line_len + 1, __FILE__, __LINE__);
        if (!new_content) {
            if (content) ft_free(content);
            ft_free(line);
            return NULL;
        }
        
        if (content) {
            ft_strcpy(new_content, content);
            ft_free(content);
        } else {
            new_content[0] = '\0';
        }
        
        ft_strcat(new_content, line);
        ft_strcat(new_content, "\n");
        content = new_content;
        content_size += line_len;
        ft_free(line);
    }
    
    // Eğer hiç line okunamadıysa boş string döndür
    if (content_size == 0) {
        return ft_strdup("");  // Boş string döndür, NULL değil
    }
    
    return content;
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
            if (line) 
                ft_free(line);
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
    
    // EOF durumu - pipe kapandığında bytes_read == 0
    if (bytes_read == 0) {
        return NULL;  // EOF - döngüden çık
    }
    
    // Normal durum - boş satır da dahil (line_size == 0 ise boş string döndür)
    if (line_size == 0) {
        return ft_strdup("");  // Boş satır için boş string
    }
    
    return line;
}

