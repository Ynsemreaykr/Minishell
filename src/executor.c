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




void setup_redirections_for_child(t_cmd *cmd, int fd_in, int *pipefd)
{
    int input_redirected  = 0;
    // 1) Önce output redirection'ları ayarla (sadece dosya redirection'ları)
    if (cmd->redirs) {
        for (t_redir *r = cmd->redirs; r; r = r->next) {
            if (r->type == REDIR_OUT) {
                fprintf(stderr,"redir out\n");

                int out = open(r->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (out >= 0) {
                    dup2(out, STDOUT_FILENO);
                    close(out);
                }
            } else if (r->type == REDIR_APPEND) {
                int out = open(r->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
                if (out >= 0) {
                    dup2(out, STDOUT_FILENO);
                    close(out);
                }
            }
        }
    }

    // 2) Sonra input redirection'ları ayarla (heredoc hariç)
    if (cmd->redirs) {
        for (t_redir *r = cmd->redirs; r; r = r->next) {
            if (r->type == REDIR_IN) {
                fprintf(stderr,"redir in\n");
                // Dosya var mı kontrol et
                if (access(r->filename, F_OK) != 0) {
                    ft_putstr_fd(r->filename, 2);
                    ft_putstr_fd(": No such file or directory\n", 2);
                    ft_mem_cleanup();
                    _exit(1);
                }
                int in = open(r->filename, O_RDONLY);
                if (in >= 0) {
                    dup2(in, STDIN_FILENO);
                    close(in);
                    input_redirected = 1;
                } else {
                    perror(r->filename);
                    ft_mem_cleanup();
                    _exit(1);
                }
            }
        }
    }

    // 3) Heredoc → stdin (varsa ve input redirect yapılmadıysa)
    if (cmd->heredocs && !input_redirected) {
        // Son heredoc'u bul
        t_heredoc *h = cmd->heredocs;
        while (h->next) h = h->next;

        if (h->content) {
            // Heredoc content'ini stdin'e yaz
            int hpipe[2];
            if (pipe(hpipe) == 0) {
                write(hpipe[1], h->content, ft_strlen(h->content));
                close(hpipe[1]);
                dup2(hpipe[0], STDIN_FILENO);
                close(hpipe[0]);
                input_redirected = 1;
            }
        }
    }
    
    // 4) PIPE FALLBACKLAR

    // Sol taraftan veri alma (input redirect uygulanmadıysa ve fd_in geçerliyse)
    if (!input_redirected && fd_in != STDIN_FILENO) {   
        dup2(fd_in, STDIN_FILENO);
        close(fd_in);
    } else {
        // input redirect yapıldıysa veya fd_in = 0 ise, var ise fd_in'i sızdırmamak adına kapat
        if (fd_in != STDIN_FILENO)
            close(fd_in);
    }

    // Pipe setup artık handle_pipeline.c'de yapılıyor
    // Burada sadece gereksiz file descriptor'ları kapat
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
