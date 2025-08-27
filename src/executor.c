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
    
    // Eğer hiç line okunamadıysa boş string döndür
    if (content_size == 0) {
        return ft_strdup("");  // Boş string döndür, NULL değil
    }
    
    return content;
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
