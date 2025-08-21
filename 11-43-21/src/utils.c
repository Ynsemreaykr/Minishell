#include "../include/minishell.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

void free_args(char **args)
{
    for (int i = 0; args && args[i]; i++)
        ft_free(args[i]);
    ft_free(args);
}



void free_cmds(t_cmd *cmd)
{
    t_cmd *tmp;
    int i;
    
    while (cmd)
    {
        tmp = cmd->next;
        free_args(cmd->argv);
        if (cmd->infile)
            ft_free(cmd->infile);
        if (cmd->outfile)
            ft_free(cmd->outfile);
        // Free heredoc structure
        if (cmd->heredoc)
        {
            if (cmd->heredoc->delimiters)
            {
                i = 0;
                while (i < cmd->heredoc->count)
                {
                    if (cmd->heredoc->delimiters[i])
                        ft_free(cmd->heredoc->delimiters[i]);
                    i++;
                }
                ft_free(cmd->heredoc->delimiters);
            }
            if (cmd->heredoc->cleaned_delimiters)
            {
                i = 0;
                while (i < cmd->heredoc->count)
                {
                    if (cmd->heredoc->cleaned_delimiters[i])
                        ft_free(cmd->heredoc->cleaned_delimiters[i]);
                    i++;
                }
                ft_free(cmd->heredoc->cleaned_delimiters);
            }
            if (cmd->heredoc->quoted_flags)
                ft_free(cmd->heredoc->quoted_flags);
            ft_free(cmd->heredoc);
        }
        

        ft_free(cmd);
        cmd = tmp;
    }
}





char *find_path(char *cmd, char **envp)
{
    char *env_path = NULL;
    char *token;
    char buf[1024];
    int i;
    if (!cmd || strchr(cmd, '/'))
        return ft_strdup(cmd);
    for (i = 0; envp[i]; i++)
        if (!ft_strncmp(envp[i], "PATH=", 5))
            env_path = envp[i] + 5;
    if (!env_path)
        return NULL;
    env_path = ft_strdup(env_path);
    char *saveptr;
    token = ft_strtok_r(env_path, ":", &saveptr);
    while (token)
    {
        ft_strcpy(buf, token);
        ft_strcat(buf, "/");
        ft_strcat(buf, cmd);
        if (access(buf, X_OK) == 0)
            return (ft_free(env_path), ft_strdup(buf));
        token = ft_strtok_r(NULL, ":", &saveptr);
    }
    ft_free(env_path);
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

// exec_child fonksiyonu artık executor.c'de tanımlı

// heredoc_input fonksiyonu kaldırıldı, multiple_heredoc_input kullanılıyor. 

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

 