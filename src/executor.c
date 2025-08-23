#include "../include/minishell.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/unistd.h>

// Heredoc content'ini oku ve string olarak döndür
static char *read_heredoc_content(int fd)
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

// 1. Fork işlemi - Yeni process yarat
static pid_t create_child_process(void)
{
    pid_t pid = fork();
    if (pid < 0) 
    {
        perror("fork");
        return -1;
    }
    return pid;
}


// Pipeline öncesi tüm heredoc'ları işle
static int first_heredoc_process(t_cmd *cmds, t_shell *shell)
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
                // Heredoc'u işle ve fd al
                int hfd = multiple_heredoc_input(heredoc, shell);
                
                if (hfd < 0) 
                {
                    // Heredoc hatası
                    return 0;
                }
                
                // Heredoc content'ini oku ve yapıda sakla
                char *content = read_heredoc_content(hfd);
                close(hfd);
                
                if (content) {
                    // Eski content varsa temizle
                    if (heredoc->content) {
                        ft_free(heredoc->content);
                    }
                    heredoc->content = content;
                }
                
                heredoc = heredoc->next;
            }
        }
        current = current->next;
    }
    
    return 1; // Başarılı
}
// Heredoc content'lerini temizle
static void cleanup_heredoc_content(t_cmd *cmds)
{
    t_cmd *current = cmds;
    
    while (current) {
        if (current->heredocs) {
            t_heredoc *heredoc = current->heredocs;
            while (heredoc) {
                if (heredoc->content) {
                    // Content'i temizle
                    ft_free(heredoc->content);
                    heredoc->content = NULL;
                }
                heredoc = heredoc->next;
            }
        }
        current = current->next;
    }
}

// 2. Dup2 işlemi - stdout/stderr/stdin yönlendir
static void setup_redirections(t_cmd *cmd, int fd_in, int *pipefd)
{
    int input_redirected = 0;
    int output_redirected = 0;
    
    // Heredoc yönlendirmesi - zaten pipeline öncesi işlendi
    if (cmd->heredocs) {
        // İlk heredoc'u kullan (en son eklenen)
        t_heredoc *current = cmd->heredocs;
        while (current->next) {
            current = current->next;
        }
        if (current->content) {
            // Heredoc content'ini stdin'e yaz
            int pipefd[2];
            if (pipe(pipefd) == 0) {
                write(pipefd[1], current->content, ft_strlen(current->content));
                close(pipefd[1]);
                dup2(pipefd[0], 0);
                close(pipefd[0]);
                input_redirected = 1;
            }
        }
    }
    
    // Redirection listesini işle (soldan sağa sırayla)
    if (cmd->redirs) {
        t_redir *current = cmd->redirs;
        while (current) {
            if (current->type == REDIR_IN) {
                // Input dosyası var mı kontrol et
                if (access(current->filename, F_OK) != 0) {
                    ft_putstr_fd(current->filename, 2);
                    ft_putstr_fd(": No such file or directory\n", 2);
                    ft_mem_cleanup();
                    _exit(1);
                }
                
                int in = open(current->filename, O_RDONLY);
                if (in < 0) { 
                    ft_mem_cleanup();
                    _exit(1);
                }
                dup2(in, 0); 
                close(in);
                input_redirected = 1;
            } else if (current->type == REDIR_OUT) {
                int out = open(current->filename, O_WRONLY|O_CREAT|O_TRUNC, 0644);
                if (out < 0) { 
                    perror(current->filename); 
                    ft_mem_cleanup(); 
                    _exit(1); 
                }
                dup2(out, 1); 
                close(out);
                output_redirected = 1;
            } else if (current->type == REDIR_APPEND) {
                int out = open(current->filename, O_WRONLY|O_CREAT|O_APPEND, 0644);
                if (out < 0) { 
                    perror(current->filename); 
                    ft_mem_cleanup(); 
                    _exit(1); 
                }
                dup2(out, 1); 
                close(out);
                output_redirected = 1;
            }
            current = current->next;
        }
    }
    
    // Pipe input yönlendirmesi (sadece input redirect edilmemişse)
    if (!input_redirected && fd_in != 0) 
    { 
        dup2(fd_in, 0); 
        close(fd_in); 
    }
    
    // Pipe output yönlendirmesi (sadece output redirect edilmemişse)
    if (!output_redirected && cmd->next) 
    { 
        dup2(pipefd[1], 1); 
        close(pipefd[1]); 
    }
    
    // Pipe'ın diğer ucunu kapat
    if (cmd->next) 
        close(pipefd[0]);
}

// 3. Execve işlemi - Komutu çalıştır
static void execute_command(t_cmd *cmd, char **envp)
{
    char *path = find_path(cmd->argv[0], envp);
    if (!path) { 
        ft_putstr_fd(cmd->argv[0], 2);
        ft_putstr_fd(": command not found\n", 2); 
        ft_mem_cleanup(); // Child process'te memory temizle
        _exit(127); 
    }
    
    // Path contains slash - check if file exists
    if (strchr(cmd->argv[0], '/')) {
        if (access(path, F_OK) != 0) {
            ft_putstr_fd(cmd->argv[0], 2);
            ft_putstr_fd(": No such file or directory\n", 2);
            ft_mem_cleanup(); // Child process'te memory temizle
            _exit(127);
        }
    }
     // Directory check
    struct stat st;
    if (stat(path, &st) == 0 && S_ISDIR(st.st_mode)) {
        ft_putstr_fd(cmd->argv[0], 2);
        ft_putstr_fd(": Is a directory\n", 2);
        ft_mem_cleanup(); // Child process'te memory temizle
        _exit(126);
    }
    
    // Dosya var ama çalıştırılabilir değilse 126 exit code'u ver
    if (access(path, X_OK) != 0) {
        ft_putstr_fd(cmd->argv[0], 2);
        ft_putstr_fd(": Permission denied\n", 2);
        ft_mem_cleanup(); // Child process'te memory temizle
        _exit(126);
    }

    execve(path, cmd->argv, envp);
    perror("execve"); 
    ft_mem_cleanup(); // Execve başarısız olduğunda memory temizle
    _exit(127);
}

// Child process'in ana fonksiyonu
static void run_child_process(t_cmd *cmd, char **envp, int fd_in, int *pipefd, t_shell *shell)
{
    // Signal handler'ları sıfırla
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    
    // minishell çinde minishell çalıştırılırsa güncel env export gibi bilgiler çocuğa geçmeli
    if (cmd->argv && cmd->argv[0] && ft_strcmp(cmd->argv[0], "./minishell") == 0) {
        // Environment'ı güncelle
        envp = shell->env;
    }
    
    // Redirections'ları ayarla
    setup_redirections(cmd, fd_in, pipefd);
    
    // Eğer komut yoksa (sadece heredoc/redirection varsa), hiçbir çıktı verme
    if (!cmd->argv || !cmd->argv[0]) {
        // Sadece heredoc/redirection varsa, hiçbir çıktı verme
        ft_mem_cleanup(); // Memory temizle
        exit(0);
    }
    
    // Önce builtin komut kontrolü yap
    if (is_builtin(cmd->argv[0])) {
        int result = exec_builtin(cmd, shell);
        // Builtin komutlar için memory temizle ve exit
        ft_mem_cleanup();
        exit(result);
    }
    
    // Builtin değilse exec ile çalıştır
    execute_command(cmd, envp);
}

// 4. Wait işlemi - Parent, child'ları bekler
static int wait_for_children(void)
{
    int status;
    pid_t last_pid = -1;
    
    while ((last_pid = wait(&status)) > 0) {
        // Son child process'in status'unu döndür
    }
    
    // Signal ile sonlanan process'ler için özel handling
    if (WIFSIGNALED(status)) {
        int sig = WTERMSIG(status);
        if (sig == SIGINT) {
            return 130; // Ctrl+C için exit code 130
        } else if (sig == SIGQUIT) {
            return 131; // Ctrl+\ için exit code 131
        }
        return 128 + sig; // Diğer signaller için 128 + signal number
    }
    
    return status;
}

// Builtin komutlar için redirection ayarları
static int setup_builtin_redirections(t_cmd *cmd, int *old_stdin, int *old_stdout, 
                                     int *outfd)
{
    int error_count = 0;
    int input_redirected = 0;
    int output_redirected = 0;
    
    // Heredoc yönlendirmesi
    if (cmd->heredocs) {
        *old_stdin = dup(0);
        
        // Heredoc zaten pipeline öncesi işlendi
        // Sadece hazır input'u kullan
        t_heredoc *current = cmd->heredocs;
        while (current->next) {
            current = current->next;
        }
        if (current->content) {
            // Heredoc content'ini stdin'e yaz
            int pipefd[2];
            if (pipe(pipefd) == 0) {
                write(pipefd[1], current->content, ft_strlen(current->content));
                close(pipefd[1]);
                dup2(pipefd[0], 0);
                close(pipefd[0]);
                input_redirected = 1;
            }
        }
    }
    
    // Redirection listesini işle (builtin için)
    if (cmd->redirs) {
        t_redir *current = cmd->redirs;
        while (current) {
            if (current->type == REDIR_IN && !input_redirected) {
                *old_stdin = dup(0);
                
                // Input dosyası var mı kontrol et
                if (access(current->filename, F_OK) != 0) {
                    ft_putstr_fd(current->filename, 2);
                    ft_putstr_fd(": No such file or directory\n", 2);
                    error_count++;
                } else {
                    int infd = open(current->filename, O_RDONLY);
                    if (infd < 0) {
                        error_count++;
                    } else {
                        dup2(infd, 0);
                        close(infd);
                        input_redirected = 1;
                    }
                }
            } else if (current->type == REDIR_OUT) {
                if (!output_redirected) {
                    *old_stdout = dup(1);
                    output_redirected = 1;
                }
                *outfd = open(current->filename, O_WRONLY|O_CREAT|O_TRUNC, 0644);
                if (*outfd < 0) { 
                    error_count++;
                } else { 
                    dup2(*outfd, 1); 
                    close(*outfd); 
                }
            } else if (current->type == REDIR_APPEND) {
                if (!output_redirected) {
                    *old_stdout = dup(1);
                    output_redirected = 1;
                }
                *outfd = open(current->filename, O_WRONLY|O_CREAT|O_APPEND, 0644);
                if (*outfd < 0) { 
                    error_count++;
                } else { 
                    dup2(*outfd, 1); 
                    close(*outfd); 
                }
            }
            current = current->next;
        }
    }
    
    return error_count;
}

// Builtin komutlar için redirection geri yükleme
static void restore_builtin_redirections(t_cmd *cmd, int old_stdin, int old_stdout)
{
    // Redirection yapıldıysa geri yükle
    if (old_stdout != -1) {
        dup2(old_stdout, 1);
        close(old_stdout);
    }
    if (old_stdin != -1) {
        dup2(old_stdin, 0);
        close(old_stdin);
    }
    (void)cmd; // Unused parameter
}

// Pipeline execution - Ana execute fonksiyonu
int exec_pipeline(t_cmd *cmds, char **envp, t_shell *shell)
{
    int fd_in = 0; // ilk komutun stdin i : terminal
    int pipefd[2]; // pipe[0] : okuma pipefd[1] : yazma ucu
    pid_t pid;
    t_cmd *cmd = cmds;
    
    // Komut çalışmaya başladı - sinyal handler'ları güncelle
    setup_command_signals();
    
    // ÖNCE: Tüm komutların heredoc'larını işle
    if (!first_heredoc_process(cmds, shell)) {
        setup_normal_signals(); // Normal sinyallere dön
        return 1; // Heredoc hatası
    }
    
    cmd = cmds; 
    while (cmd) 
    {
        // Pipe oluştur (eğer sonraki komut varsa, son pipe a geldiysek açma)
        if (cmd->next) 
            pipe(pipefd);
        
        // 1. Fork işlemi
        pid = create_child_process();
        
        if (pid == 0) 
        {
            // Child process
            run_child_process(cmd, envp, fd_in, pipefd, shell); // child lar komutu çalıştırır
        } 
        else if (pid > 0) 
        {
            // Parent process
            if (fd_in != 0)  // soldaki pipe ın okuma ucunu kapa
                close(fd_in);
            if (cmd->next) 
            { 
                close(pipefd[1]); // yeni pipe geçeceğiz yeni pipe ın yazma ucunu kapa, child yazma ucunu açacak
                fd_in = pipefd[0]; // sağdaki pipe ın okuma ucunu sakla şuanki pipe ın yazma ucuna ata. çünkü sonraki child sonraki komutta kullanacak
            }
        }
        
        cmd = cmd->next;
    }
    
    // 4. Wait işlemi
    int result = wait_for_children();
    
    // Heredoc content'lerini temizle
    cleanup_heredoc_content(cmds);
    
    // Komut tamamlandı - normal sinyallere dön
    setup_normal_signals();
    
    return result;
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

// Builtin komut execution
int exec_builtin_with_redirections(t_cmd *cmd, t_shell *shell)
{
    int old_stdin = -1, old_stdout = -1;
    int outfd = -1;
    
    // Komut çalışmaya başladı - sinyal handler'ları güncelle
    setup_command_signals();
    
    // Redirections ayarla ve hata kontrolü yap
    int redir_errors = setup_builtin_redirections(cmd, &old_stdin, &old_stdout, &outfd);
    

    
    int result = 0;
    if (redir_errors > 0) {
        result = 1; // Redirection hatası varsa exit code 1
    } else {
        // Builtin komutu çalıştır
        result = exec_builtin(cmd, shell);
    }
    
    // Redirections geri yükle
    restore_builtin_redirections(cmd, old_stdin, old_stdout);
    
    // Komut tamamlandı - normal sinyallere dön
    setup_normal_signals();
    
    return result;
} 