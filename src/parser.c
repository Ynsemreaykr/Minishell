#include "../include/minishell.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

// Check pipe syntax before parsing
static int pipe_check(const char *input)
{
    int len = ft_strlen(input);
    int i = 0;
    int in_quote = 0;
    char quote_char = 0;
    
    // Skip leading whitespace
    while (i < len && (input[i] == ' ' || input[i] == '\t')) {
        i++;
    }
    
    // Check for pipe at start
    if (i < len && input[i] == '|') {
        return 0; // Invalid: | at start
    }
    
    // Check for pipe at end
    if (len > 0 && input[len - 1] == '|') {
        return 0; // Invalid: | at end
    }
    
    // Check for consecutive pipes (only if not in quotes)
    i = 0;
    while (i < len - 1) {
        // Update quote state
        if ((input[i] == '"' || input[i] == '\'') && !in_quote) {
            quote_char = input[i];
            in_quote = 1;
        } else if (input[i] == quote_char && in_quote) {
            in_quote = 0;
            quote_char = 0;
        }
        
        // Only check pipe syntax if not in quotes
        if (input[i] == '|' && !in_quote) {
            // Skip whitespace after pipe
            int j = i + 1;
            while (j < len && (input[j] == ' ' || input[j] == '\t')) {
                j++;
            }
            // If next non-whitespace is also pipe
            if (j < len && input[j] == '|') {
                return 0; // Invalid: | |
            }
        }
        i++;
    }
    
    return 1; // Valid pipe syntax
}



// Quote type'ını detect et ve delimiter'ı temizle
static int detect_quote_type_and_clean_delimiter(char **delimiter)
{
    if (!*delimiter || !**delimiter)
        return 0; // Quote yok
    
    char *delim = *delimiter;
    int len = ft_strlen(delim);
    
    // Tek tırnak kontrolü
    if (len >= 2 && delim[0] == '\'' && delim[len-1] == '\'') {
        // Tırnakları kaldır
        char *cleaned = ft_malloc(len - 1, __FILE__, __LINE__);
        ft_strncpy(cleaned, delim + 1, len - 2);
        cleaned[len - 2] = '\0';
        ft_free(*delimiter);
        *delimiter = cleaned;
        return 1; // Tek tırnak
    }
    
    // Çift tırnak kontrolü  
    if (len >= 2 && delim[0] == '"' && delim[len-1] == '"') {
        // Tırnakları kaldır
        char *cleaned = ft_malloc(len - 1, __FILE__, __LINE__);
        ft_strncpy(cleaned, delim + 1, len - 2);
        cleaned[len - 2] = '\0';
        ft_free(*delimiter);
        *delimiter = cleaned;
        return 2; // Çift tırnak
    }
    
    return 0; // Quote yok
}



static void handle_in_redir(t_cmd *cmd, char **argv, int *i)
{
    if (argv[*i + 1]) {
        // Redirection listesine ekle
        t_redir *redir = create_redir(REDIR_IN, argv[*i + 1]);
        if (redir) {
            add_redir(cmd, redir);
        }
        
        *i+=2;
    }
    // Artış işlemi çağıran fonksiyonda yapılacak
}





static void parse_redirections_and_heredoc(t_cmd *cmd)
{
    int i = 0, j = 0;
    int argc = 0;
  
    // Kaç tane argüman kalacağını say
    i = 0;
    while (cmd->argv && cmd->argv[i]) 
    {
        if (!ft_strcmp(cmd->argv[i], ">") || !ft_strcmp(cmd->argv[i], ">>") ||
            !ft_strcmp(cmd->argv[i], "<") || !ft_strcmp(cmd->argv[i], "<<")) 
        {
            i += 2;
        } 
        else 
        {
            argc++;
            i++;
        }
    }
    // Yeni argv oluştur
    char **new_argv = ft_malloc(sizeof(char *) * (argc + 1), __FILE__, __LINE__);
    
    // Redirection'ları parse et
    i = 0;
    while (cmd->argv && cmd->argv[i]) 
    {
        if (!ft_strcmp(cmd->argv[i], ">")) 
        {
            if (cmd->argv[i + 1]) 
            {
                // Redirection listesine ekle
                t_redir *redir = create_redir(REDIR_OUT, cmd->argv[i + 1]);
                if (redir) {
                    add_redir(cmd, redir);
                }
            }
            i += 2; // Hem > operatörünü hem de dosya adını atla
        } 


        else if (!ft_strcmp(cmd->argv[i], ">>")) 
        {
            if (cmd->argv[i + 1]) 
            {
                // Redirection listesine ekle
                t_redir *redir = create_redir(REDIR_APPEND, cmd->argv[i + 1]);
                if (redir) {
                    add_redir(cmd, redir);
                }
            }
            i += 2; // Hem >> operatörünü hem de dosya adını atla
        } 
        
        
        else if (!ft_strcmp(cmd->argv[i], "<")) 
        {
            handle_in_redir(cmd, cmd->argv, &i);
            // handle_in_redir zaten i'yi artırıyor, ekstra artırmaya gerek yok
        } 
        
        else if (!ft_strcmp(cmd->argv[i], "<<")) 
        {
            if (cmd->argv[i + 1]) {
                // Delimiter'ın orijinal kopyasını sakla
                char *original_delimiter = ft_strdup(cmd->argv[i + 1]);
                // Quote type'ı detect et ve delimiter'ı temizle
                int quote_type = detect_quote_type_and_clean_delimiter(&cmd->argv[i + 1]);
                // Yeni heredoc oluştur - original ve cleaned delimiter ayrı
                t_heredoc *heredoc = create_heredoc(original_delimiter, cmd->argv[i + 1], quote_type);
                if (heredoc) {
                    add_heredoc(cmd, heredoc);
                }
                ft_free(original_delimiter);
            }
            i += 2; // Hem << operatörünü hem de delimiter'ı atla
        } 
        else 
        {
            new_argv[j++] = ft_strdup(cmd->argv[i++]);
        }
    }
    new_argv[j] = NULL;
    
    // Eski argv'yi temizle
    int k = 0;
    while (cmd->argv && cmd->argv[k]) {
        ft_free(cmd->argv[k]);
        k++;
    }
    ft_free(cmd->argv);
    
    if (j == 0) {
        // Bu yeni argv kullanılmayacak, sızdırmamak için serbest bırak
        ft_free(new_argv);
        cmd->argv = NULL;
        // Eğer sadece redirection varsa (örn: >file), boş argv oluştur
        /*if (cmd->redirs) {
            cmd->argv = ft_malloc(sizeof(char *) * 1,__FILE__, __LINE__);
            cmd->argv[0] = NULL; // Boş argv
        } else {
            cmd->argv = NULL;
        }*/
        // Eğer heredoc varsa, onu koruyalım (örn: <<aa durumu)

    } else {
        cmd->argv = new_argv;
    }
}



t_cmd *parse_commands(const char *input, int last_exit, t_shell *shell) // komutları parçalar ve t_cmd listesine ekler
{
    // Pipe syntax kontrolü - en başta
    if (!pipe_check(input)) {
        ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", 2);
        return NULL;
    }
    
    t_cmd *head = NULL;
    t_cmd *last = NULL;
    
    // Quote-aware pipe splitting kullan
    int cmd_count = 0;
    char **cmd_strings = split_by_pipes(input, &cmd_count);
    
    // Check for syntax error in split_by_pipes
    if (!cmd_strings) {
        ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", 2);
        return NULL;
    }
    
    int c = 0;
    while (c < cmd_count)
    {
        char *cmd_str = cmd_strings[c];
        
        t_cmd *cmd = ft_malloc(sizeof(t_cmd), __FILE__, __LINE__);
        ft_memset(cmd, 0, sizeof(t_cmd)); // calloc gibi sıfırla
        // Token'lara ayır
        cmd->argv = split_tokens(cmd_str, last_exit, shell); // ["echo", "selam", ">", "out.txt", NULL]
        
        // Parse error durumunda cmd'yi temizle ve devam et
        // Ama heredoc varsa (örn: <<aa durumu) bu normal
        // Ayrıca argv boş array ise (örn: $EMPTY expand edildi) bu da normal
        if (!cmd->argv && !cmd->heredocs) {
            ft_putstr_fd("minishell: syntax error near unexpected token\n", 2);
            cleanup_and_return_null(cmd, cmd_strings, cmd_count);
            return NULL;
        }
        
        // Eğer argv boş array ise (hiç token yok), boş komut olarak handle et
        if (cmd->argv && !cmd->argv[0]) {  // +
            // Boş komut - hiçbir şey yapma
            if (head == NULL) {
                head = cmd;
                last = cmd;
            } else {
                last->next = cmd;
                last = cmd;
            }
            c++;
            continue;
        }
        
        // Peş peşe redirection token kontrolü
        if (cmd->argv) {
            int i = 0;
            while (cmd->argv[i]) {
                // Redirection token kontrolü
                if ((!ft_strcmp(cmd->argv[i], ">") || !ft_strcmp(cmd->argv[i], ">>") ||
                     !ft_strcmp(cmd->argv[i], "<") || !ft_strcmp(cmd->argv[i], "<<")) &&
                    i > 0) {
                    // Önceki token da redirection mu?
                    if (!ft_strcmp(cmd->argv[i-1], ">") || !ft_strcmp(cmd->argv[i-1], ">>") ||
                        !ft_strcmp(cmd->argv[i-1], "<") || !ft_strcmp(cmd->argv[i-1], "<<")) {
                        ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
                        ft_putstr_fd(cmd->argv[i], 2);
                        ft_putstr_fd("'\n", 2);
                        cleanup_and_return_null(cmd, cmd_strings, cmd_count);
                        return NULL;
                    }
                }
                i++;
            }
        }
        

        
        parse_redirections_and_heredoc(cmd);                    // cmd->argv = ["echo", "selam", NULL]
        // Parse redirections error kontrolü
        // Ama heredoc veya redirection varsa (örn: <<aa veya >file.txt durumu) bu normal
        if (!cmd->argv && !cmd->heredocs && !cmd->redirs) {
            // Sadece argv, heredoc VE redirs yoksa syntax error
            cleanup_and_return_null(cmd, cmd_strings, cmd_count);
            return NULL;
        }

        
        cmd->next = NULL;
        
        if (!head)
            head = cmd;
        else
            last->next = cmd;
        last = cmd;
        
        c++;
    }
    
    // Clean up command strings
    c = 0;
    while (c < cmd_count) {
        ft_free(cmd_strings[c]);
        c++;
    }
    ft_free(cmd_strings);
    
    return head;
}


