#include "../include/minishell.h"
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

// Forward declarations
static int process_single_heredoc(t_heredoc *heredoc);
static void heredoc_child_process(int *pipefd, t_heredoc *heredoc);
static int heredoc_parent_process(pid_t pid, int *pipefd, t_heredoc *heredoc);

// Tüm heredoc'ları işle (child/parent ayrımı ile)
int handle_all_heredocs(t_cmd *cmds)
{
    t_cmd *current = cmds;
    
    while (current) 
    {
        if (current->heredocs) 
        {
            // Her heredoc için ayrı ayrı işlem yap
            t_heredoc *heredoc = current->heredocs;
            while (heredoc) 
            {
                // Heredoc'u child process'te input al, parent process'te content oku
                int result = process_single_heredoc(heredoc);

                if (result != 0) 
                {
                    // Heredoc hatası - tüm heredoc content'lerini temizle
                    cleanup_heredoc_content(cmds);
                    return -1;
                }

                heredoc = heredoc->next;
            }
        }
        current = current->next;
    }
    
    return 1; // Başarılı
}

// Single builtin komutu çalıştır (parent process'te)
int execute_single_builtin(t_cmd *cmd, t_shell *shell)
{
    int old_stdin = dup(STDIN_FILENO);
    int old_stdout = dup(STDOUT_FILENO);
    int result = 0;
    
    // Builtin komutlar parent process'te çalıştığı için sinyal handling gerekmez
    // Normal sinyaller zaten ayarlı
    
    // Redirection'ları ayarla
    if (setup_redirections_for_builtin(cmd) == -1) {
        result = 1;
    } else {
        // Builtin'i çalıştır
        result = exec_builtin(cmd, shell);
    }
    
    // File descriptor'ları geri yükle
    dup2(old_stdin, STDIN_FILENO);
    dup2(old_stdout, STDOUT_FILENO);
    close(old_stdin);
    close(old_stdout);
    
    return result;
}

// Single external komutu çalıştır (child process'te)
int execute_single_external(t_cmd *cmd, t_shell *shell)
{
    fprintf(stderr,"single external\n");
    pid_t pid = create_child_process();
    
    if (pid == 0) {
        // Child process
        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        
        // Redirection'ları ayarla
        setup_redirections_for_child(cmd, STDIN_FILENO, NULL);
        
        // Komutu exec ile çalıştır
        if (cmd->argv && cmd->argv[0]) {
            execute_command(cmd, shell->env);
        }
        
        cleanup_shell_for_child(shell);
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

// Heredoc'u child process'te input al, parent process'te content oku
static int process_single_heredoc(t_heredoc *heredoc)
{
    int pipefd[2];
    pid_t pid;
    
    if (pipe(pipefd) == -1)
        return -1;
    
    // Parent process'te SIGINT'i ignore et
    signal(SIGINT, SIG_IGN);
    
    pid = fork();
    if (pid == -1) {
        close(pipefd[0]);
        close(pipefd[1]);
        return -1;
    }

    
    if (pid == 0) {
        // Child process - heredoc input'u al
        heredoc_child_process(pipefd, heredoc);
    } else {
        // Parent process - content'i oku
        return heredoc_parent_process(pid, pipefd, heredoc);
    }
    return 0;
}

// Heredoc child process - input alır
static void heredoc_child_process(int *pipefd, t_heredoc *heredoc)
{
    char *line;
    
    // Child process'te sinyal handler'ları ayarla
    signal(SIGINT, heredoc_interrupt_handler);
    signal(SIGQUIT, SIG_IGN);
    
    close(pipefd[0]); // Okuma ucunu kapat
    
    while (1) {
        line = readline("> ");
        if (g_signal_number || !line || ft_strcmp(line, heredoc->delimiter) == 0) {
            if (!line && !g_signal_number)
                printf("minishell: here-document delimited by end-of-file\n");
            ft_free(line);
            break;
        }
        
        // Line'ı pipe'a yaz
        write(pipefd[1], line, ft_strlen(line));
        write(pipefd[1], "\n", 1);
        ft_free(line);
    }
    
    close(pipefd[1]);
    
    if (g_signal_number)
        cleanup_and_exit(130);
    cleanup_and_exit(0);
}

// Heredoc parent process - content'i okur
static int heredoc_parent_process(pid_t pid, int *pipefd, t_heredoc *heredoc)
{
    int status;
    
    close(pipefd[1]); // Yazma ucunu kapat
    
    // Content'i pipe'dan oku ve yapıda sakla  
    char *content = read_heredoc_content(pipefd[0]);
    
    close(pipefd[0]);
    
    // Hata kontrolü - content NULL ise
    if (content == NULL) {
        // Child process'i bekle ve temizle
        waitpid(pid, &status, 0);
        setup_normal_signals();
        return -1; // Hata durumu
    }
    
    // Eski content varsa temizle
    if (heredoc->content) {
        ft_free(heredoc->content);
    }
    
    // Yeni content'i ata
    heredoc->content = content;
    
    // Child process'i bekle
    waitpid(pid, &status, 0);
    
    // Normal sinyallere dön
    setup_normal_signals();
    
    if (WIFEXITED(status) && WEXITSTATUS(status) == 130) {
        // Ctrl+C ile sonlanan heredoc için content'i temizle
        if (heredoc->content) {
            ft_free(heredoc->content);
            heredoc->content = NULL;
        }
        return 130;
    }
    
    return 0;
}

// Pipeline komutlarını çalıştır
int execute_pipeline(t_cmd *cmds, t_shell *shell)
{
    // Pipeline'lar özel olarak handle_pipeline.c'de işlenir
    return exec_pipeline(cmds, shell);
}

// Ana execute fonksiyonu
int execute_command_main(t_cmd *cmds, t_shell *shell)
{
    fprintf(stderr,"heredoclar işleniyor\n");
    // 1) Önce tüm heredoc'ları işle
    if (handle_all_heredocs(cmds) == -1) {
        return -1;
    }
    // 2) Komut türünü belirle ve uygun executor'ı çağır
    if (cmds->next) {
        // Pipeline - birden fazla komut
        return execute_pipeline(cmds, shell);
    } else if (cmds->argv && cmds->argv[0] && is_builtin(cmds->argv[0])) {
        // Single builtin - parent process'te
        int result = execute_single_builtin(cmds, shell);
        shell->last_exit = result;
        update_last_arg(cmds->argv, shell);
        return (result == 0 ? 1 : 0);
    } else {
        // Single external - child process'te
        int result = execute_single_external(cmds, shell);
        shell->last_exit = result;
        
        if (cmds->argv) {
            update_last_arg(cmds->argv, shell);
        }
        return (result == 0 ? 1 : 0);
    }
} 