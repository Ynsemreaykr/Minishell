#include "../include/minishell.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/unistd.h>


void setup_redirections_for_child(t_cmd *cmd, int fd_in, int *pipefd)
{
    int input_redirected = 0;
    int output_redirected = 0;
    
    // Tüm yönlendirmeleri ve heredoc'ları pozisyona göre işle
    // En son gelen kazansın mantığı
    
    // 1. Önce tüm pozisyonları bul
    int max_position = 0;
    
    // Redirections pozisyonlarını bul
    for (t_redir *r = cmd->redirs; r; r = r->next) {
        if (r->position > max_position)
            max_position = r->position;
    }
    
    // Heredocs pozisyonlarını bul
    for (t_heredoc *h = cmd->heredocs; h; h = h->next) {
        if (h->position > max_position)
            max_position = h->position;
    }
    
    // 2. Sondan başa doğru işle (sonuncu kazanan)
    for (int pos = max_position; pos >= 0; pos--) {
        
        // Bu pozisyonda redirection var mı?
        for (t_redir *r = cmd->redirs; r; r = r->next) {
            if (r->position == pos) {
                if (r->type == REDIR_OUT && !output_redirected) {
                    // Output redirection (>)
                    int out = open(r->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (out < 0) {
                        perror(r->filename);
                        ft_mem_cleanup();
                        exit(1);
                    }
                    dup2(out, STDOUT_FILENO);
                    close(out);
                    output_redirected = 1;
                } else if (r->type == REDIR_APPEND && !output_redirected) {
                    // Append redirection (>>)
                    int out = open(r->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
                    if (out < 0) {
                        perror(r->filename);
                        ft_mem_cleanup();
                        exit(1);
                    }
                    dup2(out, STDOUT_FILENO);
                    close(out);
                    output_redirected = 1;
                } else if (r->type == REDIR_IN && !input_redirected) {
                    // Input redirection (<)
                    if (access(r->filename, F_OK) != 0) {
                        ft_putstr_fd("minishell : No such file or directory\n", 2);
                        ft_mem_cleanup();
                        exit(1);
                    }
                    int in = open(r->filename, O_RDONLY);
                    if (in >= 0) {
                        dup2(in, STDIN_FILENO);
                        close(in);
                        input_redirected = 1;
                    } else {
                        perror(r->filename);
                        ft_mem_cleanup();
                        exit(1);
                    }
                }
                break; // Bu pozisyonda bulduk, sonrakine geç
            }
        }
        
        // Bu pozisyonda heredoc var mı?
        for (t_heredoc *h = cmd->heredocs; h; h = h->next) {
            if (h->position == pos && !input_redirected) {
                // Heredoc (<<)
                if (h->content) {
                    int hpipe[2];
                    if (pipe(hpipe) == 0) {
                        write(hpipe[1], h->content, ft_strlen(h->content));
                        close(hpipe[1]);
                        dup2(hpipe[0], STDIN_FILENO);
                        close(hpipe[0]);
                        input_redirected = 1;
                    }
                }
                break; // Bu pozisyonda bulduk, sonrakine geç
            }
        }
    }
    
    // 3. Pipe setup
    if (!input_redirected && fd_in != STDIN_FILENO) {   
        dup2(fd_in, STDIN_FILENO);
        close(fd_in);
    } else {
        if (fd_in != STDIN_FILENO)
            close(fd_in);
    }
    
    if (cmd->next) {
        close(pipefd[0]);
        close(pipefd[1]);
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
