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

// Quote'ları kaldır fonksiyonu - redirection dosya isimleri için
static char *remove_quotes(const char *str)
{
    if (!str)
        return NULL;
        
    int len = ft_strlen(str);
    if (len == 0)
        return ft_strdup(str);
    
    // Eğer string tek tırnak ile başlayıp bitiyorsa
    if (len >= 2 && str[0] == '\'' && str[len-1] == '\'') {
        char *result = ft_malloc(len - 1, __FILE__, __LINE__);
        ft_strncpy(result, str + 1, len - 2);
        result[len - 2] = '\0';
        return result;
    }
    
    // Eğer string çift tırnak ile başlayıp bitiyorsa
    if (len >= 2 && str[0] == '"' && str[len-1] == '"') {
        char *result = ft_malloc(len - 1, __FILE__, __LINE__);
        ft_strncpy(result, str + 1, len - 2);
        result[len - 2] = '\0';
        return result;
    }
    
    // Quote yok, orijinal string'i kopyala
    return ft_strdup(str);
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

// Heredoc sayısını say
static int count_heredocs_in_argv(char **argv)
{
    if (!argv)
        return 0;
    
    int count = 0;
    int i = 0;
    
    while (argv[i]) {
        if (!ft_strcmp(argv[i], "<<")) {
            count++;
        }
        i++;
    }
    
    return count;
}

static void handle_in_redir(t_cmd *cmd, char **argv, int *i)
{
    if (argv[*i + 1]) {
        // Önceki infile'ı temizle (multiple redirection için)
        if (cmd->infile) {
            ft_free(cmd->infile);
        }
        cmd->infile = ft_strdup(argv[*i + 1]);
        
        *i += 2;
    }
}



// Clean up resources and return NULL on syntax error
static void cleanup_and_return_null(t_cmd *cmd, char **cmd_strings, int cmd_count)
{
    if (cmd) {
        // Önce cmd->argv içindeki token'ları free et
        if (cmd->argv) {
            int i = 0;
            while (cmd->argv[i]) {
                ft_free(cmd->argv[i]);
                i++;
            }
            ft_free(cmd->argv);
        }
        
        // Heredoc temizliği
        if (cmd->heredoc) {
            if (cmd->heredoc->delimiters) {
                int i = 0;
                while (i < cmd->heredoc->count && cmd->heredoc->delimiters[i]) {
                    ft_free(cmd->heredoc->delimiters[i]);
                    i++;
                }
                ft_free(cmd->heredoc->delimiters);
            }
            if (cmd->heredoc->cleaned_delimiters) {
                int i = 0;
                while (i < cmd->heredoc->count && cmd->heredoc->cleaned_delimiters[i]) {
                    ft_free(cmd->heredoc->cleaned_delimiters[i]);
                    i++;
                }
                ft_free(cmd->heredoc->cleaned_delimiters);
            }
            if (cmd->heredoc->quoted_flags) {
                ft_free(cmd->heredoc->quoted_flags);
            }
            if (cmd->heredoc->temp_filename) {
                ft_free(cmd->heredoc->temp_filename);
            }
            ft_free(cmd->heredoc);
        }
        
        // Redirection dosya isimlerini temizle
        if (cmd->infile) {
            ft_free(cmd->infile);
        }
        if (cmd->outfile) {
            ft_free(cmd->outfile);
        }
        
        ft_free(cmd);
    }
    // Tüm command'ları temizle
    int k=0;
    while(k < cmd_count) {
        ft_free(cmd_strings[k]);
        k++;
    }
    ft_free(cmd_strings);
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
                if (cmd->outfile) // eğer outfile varsa free lenir çünkü birden fazla girdide
                    ft_free(cmd->outfile); // echo hello > yunus.txt > emre.txt => son dosyaya yani emre.txt ye hello yazılır
                cmd->outfile = ft_strdup(cmd->argv[i + 1]);
                cmd->append = 0;
            }
            i += 2;
        } 


        else if (!ft_strcmp(cmd->argv[i], ">>")) 
        {
            if (cmd->argv[i + 1]) 
            {
                if (cmd->outfile) 
                    ft_free(cmd->outfile);
                cmd->outfile = ft_strdup(cmd->argv[i + 1]);
                cmd->append = 1;
            }
            i += 2;
        } 
        
        
        else if (!ft_strcmp(cmd->argv[i], "<")) 
        {
            handle_in_redir(cmd, cmd->argv, &i);
        } 
        
        else if (!ft_strcmp(cmd->argv[i], "<<")) 
        {
            // t_heredoc yapısını hazırla
            if (!cmd->heredoc)
            {
                cmd->heredoc = ft_malloc(sizeof(t_heredoc), __FILE__, __LINE__);
                 ft_memset(cmd->heredoc, 0, sizeof(t_heredoc));
                // Heredoc sayısını başta hesapla
                int heredoc_count = count_heredocs_in_argv(cmd->argv);
                if (heredoc_count > 0)
               {
                        cmd->heredoc->delimiters = ft_malloc(sizeof(char *) * heredoc_count, __FILE__, __LINE__);
                        cmd->heredoc->cleaned_delimiters = ft_malloc(sizeof(char *) * heredoc_count, __FILE__, __LINE__);
                        cmd->heredoc->quoted_flags = ft_malloc(sizeof(int) * heredoc_count, __FILE__, __LINE__);
                }
            }
            cmd->heredoc->enabled = 1;
            int quote_type = detect_quote_type_and_clean_delimiter(&cmd->argv[i + 1]);
            cmd->heredoc->delimiters[cmd->heredoc->count] = ft_strdup(cmd->argv[i + 1]);
            cmd->heredoc->cleaned_delimiters[cmd->heredoc->count] = ft_strdup(cmd->argv[i + 1]);
            cmd->heredoc->quoted_flags[cmd->heredoc->count] = quote_type;
            cmd->heredoc->count++;
            
            i += 2;
        } else {
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
        
        // Eğer heredoc varsa, onu koruyalım (örn: <<aa durumu)
        if (cmd->heredoc && cmd->heredoc->enabled) {
            // Heredoc'u koruyoruz, diğer redirection'ları temizliyoruz
            if (cmd->infile) {
                ft_free(cmd->infile);
                cmd->infile = NULL;
            }
            if (cmd->outfile) {
                ft_free(cmd->outfile);
                cmd->outfile = NULL;
            }
        } else {
            // Clean up redirection file names since no command arguments remain
            if (cmd->infile) {
                ft_free(cmd->infile);
                cmd->infile = NULL;
            }
            if (cmd->outfile) {
                ft_free(cmd->outfile);
                cmd->outfile = NULL;
            }
            
            // Clean up heredoc memory since no command arguments remain
            if (cmd->heredoc) {
                if (cmd->heredoc->count > 0) {
                    int k = 0;
                    while (k < cmd->heredoc->count) {
                        ft_free(cmd->heredoc->delimiters[k]);
                        ft_free(cmd->heredoc->cleaned_delimiters[k]);
                        k++;
                    }
                    ft_free(cmd->heredoc->delimiters);
                    ft_free(cmd->heredoc->cleaned_delimiters);
                    ft_free(cmd->heredoc->quoted_flags);
                }
                ft_free(cmd->heredoc);
                cmd->heredoc = NULL;
            }
        }
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
        if (!cmd->argv && !(cmd->heredoc && cmd->heredoc->enabled)) {
            ft_putstr_fd("minishell: syntax error near unexpected token\n", 2);
            cleanup_and_return_null(cmd, cmd_strings, cmd_count);
            return NULL;
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
        // Ama heredoc varsa (örn: <<aa durumu) bu normal
        if (!cmd->argv && !(cmd->heredoc && cmd->heredoc->enabled)) {
            // Syntax error durumunda cmd'yi temizle ve devam et
            cleanup_and_return_null(cmd, cmd_strings, cmd_count);
            return NULL;
        }
        
        // Quote'ları kaldır (sadece redirection dosya isimleri için)
        if (cmd->infile) {
            char *unquoted = remove_quotes(cmd->infile);
            ft_free(cmd->infile);
            cmd->infile = unquoted;
        }
        
        if (cmd->outfile) {
            char *unquoted = remove_quotes(cmd->outfile);
            ft_free(cmd->outfile);
            cmd->outfile = unquoted;
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

// Check if a command is a variable assignment (VAR=value format)
int is_variable_assignment(char **argv)
{
    if (!argv || !argv[0])
        return 0;
    
    // Check if the first argument contains '=' and is not a command
    char *equals = ft_strchr(argv[0], '=');
    if (!equals || equals == argv[0])
        return 0; // No '=' or starts with '='
    
    // Check if everything before '=' is a valid variable name
    for (char *p = argv[0]; p < equals; p++) {
        if (!ft_isalnum(*p) && *p != '_')
            return 0;
    }
    
    return 1; // Valid variable assignment
}

// Execute variable assignment
int execute_variable_assignment(char **argv, t_shell *shell)
{
    if (!argv || !argv[0])
        return 1;
        
    char *equals = ft_strchr(argv[0], '=');
    if (!equals)
        return 1;
    
    // Split into variable name and value
    int name_len = equals - argv[0];
    char *var_name = ft_malloc(name_len + 1, __FILE__, __LINE__);
    ft_strncpy(var_name, argv[0], name_len);
    var_name[name_len] = '\0';
    
    char *var_value = equals + 1;
    
    // Set the environment variable using the shell's env management
    set_env_var(var_name, var_value, shell);
    
    ft_free(var_name);
    return 0;
}
