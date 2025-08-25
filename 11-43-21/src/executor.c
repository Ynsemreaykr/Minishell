#include "../include/minishell.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/unistd.h>

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
    
    return content;
}




void setup_redirections_for_child(t_cmd *cmd, int fd_in, int *pipefd)
{
    int input_redirected  = 0;
    int output_redirected = 0;

    // 1) Heredoc → stdin (varsa)
    setup_heredoc_stdin(cmd, &input_redirected);

    // 2) Redirection listesi ( <, >, >> )
    setup_redir_list(cmd, &input_redirected, &output_redirected);

    // 3) PIPE FALLBACKLAR (ayrı fonksiyon yok; inline)

    // Sol taraftan veri alma (input redirect uygulanmadıysa ve fd_in geçerliyse)
    if (!input_redirected && fd_in != STDIN_FILENO) {
        dup2(fd_in, STDIN_FILENO);
        close(fd_in);
    } else {
        // input redirect yapıldıysa veya fd_in = 0 ise, var ise fd_in'i sızdırmamak adına kapat
        if (fd_in != STDIN_FILENO)
            close(fd_in);
    }

    // Sağ tarafa veri verme (output redirect uygulanmadıysa ve sağda komut varsa)
    if (!output_redirected && cmd->next) {
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
    } else {
        // output redirect yapıldıysa, parent'tan miras alınan yazma ucunu sızdırmamak için kapat
        if (cmd->next)
            close(pipefd[1]);
    }

    // Bu child için gereksiz olan okuma ucunu kapat (sağda komut varsa)
    if (cmd->next) {
        close(pipefd[0]);
    }
}

    






// Builtin komutları çalıştır
int exec_builtin(t_cmd *cmd, t_shell *shell)
{
    // Sadece heredoc/redirection varsa
    if (!cmd->argv || !cmd->argv[0])
        return 0;
        
    if (ft_strcmp(cmd->argv[0], "echo") == 0)
        return ft_echo(cmd->argv);
    if (ft_strcmp(cmd->argv[0], "exit") == 0)
        return ft_exit(cmd->argv);
    if (ft_strcmp(cmd->argv[0], "pwd") == 0)
        return ft_pwd(cmd->argv);
    if (ft_strcmp(cmd->argv[0], "env") == 0)
        return ft_env(cmd->argv, shell);
    if (ft_strcmp(cmd->argv[0], "cd") == 0)
        return ft_cd(cmd->argv, shell);
    if (ft_strcmp(cmd->argv[0], "export") == 0)
        return ft_export(cmd->argv, shell);
    if (ft_strcmp(cmd->argv[0], "unset") == 0)
        return ft_unset(cmd->argv, shell);
    return 1;
}
