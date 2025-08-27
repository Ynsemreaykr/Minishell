#include "../include/minishell.h"
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>





// Single external komutu çalıştır (child process'te)
int execute_single_external(t_cmd *cmd, t_shell *shell)
{
    pid_t pid = create_child_process();
    
    if (pid == 0) {
        // Child process
        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGPIPE, SIG_IGN); // SIGPIPE'ı ignore et
        
        // Redirection'ları ayarla
        setup_redirections_for_child(cmd, STDIN_FILENO, NULL);
        
        // Komutu exec ile çalıştır
        if (cmd->argv && cmd->argv[0]) {
            execute_command(cmd, shell->env);
        }
        
        ft_mem_cleanup();
        exit(0);
    } else if (pid > 0) {
        // Parent process - child'ı bekle
        signal(SIGINT, SIG_IGN);
        signal(SIGQUIT, SIG_IGN);
        
        int status;
        waitpid(pid, &status, 0);
        
        // Signal ile sonlanan process'ler için özel handling
        if (WIFSIGNALED(status)) {
            int sig = WTERMSIG(status);
            if (sig == SIGINT) {
                // Ctrl+C için yeni satıra geç (son_shell gibi)
                write(STDOUT_FILENO, "\n", 1);
            }
        }
        
        // Normal sinyallere dön
        setup_normal_signals();
        
        // Heredoc content'lerini temizle
        cleanup_heredoc_content(cmd);
        
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        } else if (WIFSIGNALED(status)) {
            int sig = WTERMSIG(status);
            if (sig == SIGINT) {
                return 130; // Ctrl+C için exit code 130
            } else if (sig == SIGQUIT) {
                return 131; // Ctrl+\ için exit code 131
            }
            return 128 + sig; // Diğer signaller için 128 + signal number
        }
        
        return 1; // Hata durumu
    }
    
    return 1; // Fork hatası
}



void setup_redirections_for_child(t_cmd *cmd, int fd_in, int *pipefd)
{
    int input_redirected = 0;

    // 1) Önce output redirection'ları ayarla
    if (cmd->redirs) {
        for (t_redir *r = cmd->redirs; r; r = r->next) {
            if (r->type == REDIR_OUT) {
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

    // 2) Input redirection'ları ayarla (file input)
    if (cmd->redirs) {
        for (t_redir *r = cmd->redirs; r; r = r->next) {
            if (r->type == REDIR_IN) {
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

    // 3) Heredoc → stdin (sadece input redirect yapılmadıysa)
    if (cmd->heredocs && !input_redirected) {
        // Son heredoc'u bul
        t_heredoc *h = cmd->heredocs;
        while (h->next) h = h->next;

        if (h->content) {
            // Heredoc content'ini direkt stdin'e yaz (pipe olmadan)
            // Bu child process'te olduğu için stdin'i değiştirmek güvenli
            int temp_pipe[2];
            if (pipe(temp_pipe) == 0) {
                write(temp_pipe[1], h->content, ft_strlen(h->content));
                close(temp_pipe[1]);
                dup2(temp_pipe[0], STDIN_FILENO);
                close(temp_pipe[0]);
                input_redirected = 1;
            }
        }
    }
    
    // 4) Pipeline input handling
    // Eğer input redirect yapılmadıysa ve pipeline'dan gelen fd varsa
    if (!input_redirected && fd_in != STDIN_FILENO) {   
        dup2(fd_in, STDIN_FILENO);
        close(fd_in);
    } else if (fd_in != STDIN_FILENO) {
        // Input redirect yapıldıysa fd_in'i kapat
        close(fd_in);
    }

    // 5) Pipeline pipe'larını kapat (bu komut için gerekli değilse)
    if (pipefd && cmd->next) {
        // Bu komut son komut değilse, pipe'ları kapat
        close(pipefd[0]);
        close(pipefd[1]);
    }
}