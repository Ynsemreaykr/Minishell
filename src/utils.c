#include "../include/minishell.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

void free_args(char **args)
{
    for (int i = 0; args && args[i]; i++)
        ft_free(args[i]);
    ft_free(args);
}

// Cleanup yap ve exit
void cleanup_and_exit(int exit_code)
{
    ft_mem_cleanup();
    exit(exit_code);
}




void free_cmds(t_cmd *cmd)
{
    t_cmd *tmp;
    
    while (cmd)
    {
        tmp = cmd->next;
        free_args(cmd->argv);
        
        // Redirection listesini temizle
        if (cmd->redirs) {
            free_redir_list(cmd->redirs);
        }
        // Free heredoc structure
        if (cmd->heredocs)
        {
            free_heredoc_list(cmd->heredocs);
        }
        

        ft_free(cmd);
        cmd = tmp;
    }
}





char **parse_path(char **envp)
{
    int i;
    char *env_path = NULL;
    
    for (i = 0; envp[i]; i++) {
        if (!ft_strncmp(envp[i], "PATH=", 5)) {
            env_path = envp[i] + 5;
            break;
        }
    }
    
    if (!env_path)
        return NULL;
    
    return ft_split(env_path, ':');
}

static int search_in_path(char *command, char **splitted_path, char **full_path)
{
    int i;
    char *temp_path;
    
    if (!splitted_path)
        return -1;
    
    i = 0;
    while (splitted_path[i]) {
        temp_path = ft_strjoin(splitted_path[i], "/");
        *full_path = ft_strjoin(temp_path, command);
        ft_free(temp_path);
        
        if (access(*full_path, X_OK) == 0)
            return 0;
        
        ft_free(*full_path);
        i++;
    }
    
    *full_path = NULL;
    return -1;
}

int is_accessable(char *command, char **splitted_path, char **full_path)
{
    struct stat path_stat;
    
    if (ft_strchr(command, '/')) 
    {
        if (stat(command, &path_stat) == 0) 
        {
            if (S_ISDIR(path_stat.st_mode))
                return -3;
        }
        if (access(command, F_OK) != 0)
            return -4; 
        if (access(command, X_OK) != 0)
            return -2; 
        
        *full_path = ft_strdup(command);
        return 0;
    }
    
    return search_in_path(command, splitted_path, full_path);
}

// Eski find_path fonksiyonu - geriye uyumluluk için
char *find_path(char *cmd, char **envp)
{
    char **splitted_path = parse_path(envp);
    char *full_path = NULL;
    int result;
    
    if (!cmd || ft_strlen(cmd) == 0) {
        if (splitted_path)
            ft_split_free(splitted_path);
        return NULL;
    }
    
    result = is_accessable(cmd, splitted_path, &full_path);
    
    if (splitted_path)
        ft_split_free(splitted_path);
    
    if (result == 0)
        return full_path;
    
    return NULL;
}

void update_last_arg(char **argv, t_shell *shell)
{
    if (!argv || !argv[0])
        return;
    
    // Son argümanı bul
    int i = 0;
    while (argv[i])
        i++;
    
    if (i > 0) {
        // Önceki değeri temizle
        if (shell->last_arg)
            ft_free(shell->last_arg);
        // Son argümanı kopyala
        shell->last_arg = ft_strdup(argv[i - 1]);
    } else {
        // Eğer argüman yoksa last_arg'ı temizle
        if (shell->last_arg) {
            ft_free(shell->last_arg);
            shell->last_arg = NULL;
        }
    }
}

// Tırnak kontrolü için genel fonksiyon
int check_quotes(const char *input)
{
    int in_single_quote = 0;
    int in_double_quote = 0;
    int i = 0;
    
    while (input[i]) 
    {
        if (input[i] == '\'' && !in_double_quote) 
            in_single_quote = !in_single_quote;
        
        else if (input[i] == '"' && !in_single_quote) 
            in_double_quote = !in_double_quote;
        
        i++;
    }
    
    // Hata mesajlarını yazdır
    if (in_single_quote) {
        ft_putstr_fd("minishell: syntax error: unclosed single quote\n", 2);
    } else if (in_double_quote) {
        ft_putstr_fd("minishell: syntax error: unclosed double quote\n", 2);
    }
    
    return !(in_single_quote || in_double_quote);
}



// Redirection syntax kontrolü ve hata mesajı yazdırma
int check_redirection_syntax(const char *input)
{
    int i = 0;
    int in_quote = 0;
    char quote_char = 0;
    
    while (input[i]) 
    {
        if ((input[i] == '"' || input[i] == '\'') && !in_quote) 
        {
            quote_char = input[i];
            in_quote = 1;
            i++;
        } 
        else if (input[i] == quote_char && in_quote) 
        {
            in_quote = 0;
            quote_char = 0;
            i++;
        } 
        else if (!in_quote && (input[i] == '>' || input[i] == '<')) 
        {
            // Redirection operatörü bulundu
            int redir_count = 0;
            char redir_char = input[i];
            
            // Kaç tane > veya < var say
            while (input[i] == redir_char) 
            {
                redir_count++;
                i++;
            }
            
            // Geçersiz redirection operatörleri kontrol et
            if (redir_count > 2) {
                ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
                for (int j = 0; j < redir_count; j++) {
                    ft_putstr_fd(&redir_char, 2);
                }
                ft_putstr_fd("'\n", 2);
                return 0; // Syntax error
            }
            
            // < ve > karışık kullanım kontrol et
            if (redir_count == 2 && redir_char == '<' && input[i] == '>') {
                ft_putstr_fd("minishell: syntax error near unexpected token `newline'\n", 2);
                return 0; // <<> geçersiz
            }
            
            if (redir_count == 2 && redir_char == '>' && input[i] == '<') {
                ft_putstr_fd("minishell: syntax error near unexpected token `newline'\n", 2);
                return 0; // >>< geçersiz
            }
        } 
        else {
            i++;
        }
    }
    
    // Input'un sonunda redirection operatörü varsa syntax hatası
    if (i > 0 && (input[i-1] == '>' || input[i-1] == '<')) { // KALDIRILABİLİR
        ft_putstr_fd("minishell: syntax error near unexpected token `newline'\n", 2);
        return 0; // Syntax error
    }
        
    return 1; // Syntax geçerli
}

// Redirection yardımcı fonksiyonları
t_redir *create_redir(t_redir_type type, char *filename, int position)
{
    t_redir *redir = ft_malloc(sizeof(t_redir), __FILE__, __LINE__);
    if (!redir)
        return NULL;
    
    redir->type = type;
    redir->filename = ft_strdup(filename);
    redir->position = position;
    redir->next = NULL;
    
    return redir;
}

void add_redir(t_cmd *cmd, t_redir *redir)
{
    if (!cmd || !redir)
        return;
    
    // Liste sonuna ekle (sıra korunur)
    if (!cmd->redirs) {
        cmd->redirs = redir;
    } else {
        t_redir *current = cmd->redirs;
        while (current->next) {
            current = current->next;
        }
        current->next = redir;
    }
}

void free_redir_list(t_redir *redirs)
{
    t_redir *current = redirs;
    t_redir *next;
    
    while (current) {
        next = current->next;
        if (current->filename)
            ft_free(current->filename);
        ft_free(current);
        current = next;
    }
}

// Heredoc yardımcı fonksiyonları
t_heredoc *create_heredoc(char *delimiter, char *cleaned_delimiter, int quoted_flag, int position)
{
    t_heredoc *heredoc = ft_malloc(sizeof(t_heredoc), __FILE__, __LINE__);
    if (!heredoc)
        return NULL;
    heredoc->delimiter = ft_strdup(delimiter);
    heredoc->cleaned_delimiter = ft_strdup(cleaned_delimiter);
    heredoc->quoted_flag = quoted_flag;
    heredoc->position = position;
    heredoc->content = NULL;
    heredoc->next = NULL;
    
    return heredoc;
}

void add_heredoc(t_cmd *cmd, t_heredoc *heredoc)
{
    if (!cmd || !heredoc)
        return;
    
    // Liste sonuna ekle (sıra korunur)
    if (!cmd->heredocs) {
        cmd->heredocs = heredoc;
    } else {
        t_heredoc *current = cmd->heredocs;
        while (current->next) {
            current = current->next;
        }
        current->next = heredoc;
    }
}

void free_heredoc_list(t_heredoc *heredocs)
{
    t_heredoc *current = heredocs;
    t_heredoc *next;
    
    while (current) {
        next = current->next;
        if (current->delimiter)
            ft_free(current->delimiter);
        if (current->cleaned_delimiter)
            ft_free(current->cleaned_delimiter);
        if (current->content)
            ft_free(current->content);
        ft_free(current);
        current = next;
    }
}

int count_heredocs(t_heredoc *heredocs)
{
    int count = 0;
    t_heredoc *current = heredocs;
    
    while (current) {
        count++;
        current = current->next;
    }
    
    return count;
}