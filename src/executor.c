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
    int input_redirected  = 0;
    // 1) Önce output redirection'ları ayarla (sadece dosya redirection'ları)
    if (cmd->redirs) {
        for (t_redir *r = cmd->redirs; r; r = r->next) {
            if (r->type == REDIR_OUT) {

                int out = open(r->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (out >= 0) {
                    dup2(out, STDOUT_FILENO);
                    close(out);
                } else {
                    ft_putstr_fd("minishell: ", 2);
                    ft_putstr_fd(r->filename, 2);
                    ft_putstr_fd(": No such file or directory\n", 2);
                    ft_mem_cleanup();
                    exit(1);
                }
            } else if (r->type == REDIR_APPEND) {
                int out = open(r->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
                if (out >= 0) {
                    dup2(out, STDOUT_FILENO);
                    close(out);
                } else {
                    ft_putstr_fd("minishell: ", 2);
                    ft_putstr_fd(r->filename, 2);
                    ft_putstr_fd(": No such file or directory\n", 2);
                    ft_mem_cleanup();
                    exit(1);
                }
            }
        }
    }

    // 2) Sonra input redirection'ları ayarla (heredoc hariç)
    if (cmd->redirs) {
        for (t_redir *r = cmd->redirs; r; r = r->next) {
            if (r->type == REDIR_IN) {
                // Dosya var mı kontrol et
                if (access(r->filename, F_OK) != 0) {
                    ft_putstr_fd("minishell: ", 2);
                    ft_putstr_fd(r->filename, 2);
                    ft_putstr_fd(": No such file or directory\n", 2);
                    ft_mem_cleanup();
                    exit(1);
                }
                int in = open(r->filename, O_RDONLY);
                if (in >= 0) {
                    dup2(in, STDIN_FILENO);
                    close(in);
                    input_redirected = 1;
                } else {
                    ft_putstr_fd("minishell: ", 2);
                    ft_putstr_fd(r->filename, 2);
                    ft_putstr_fd(": No such file or directory\n", 2);
                    ft_mem_cleanup();
                    exit(1);
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

    






