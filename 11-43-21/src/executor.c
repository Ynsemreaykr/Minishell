#include "../include/minishell.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/unistd.h>

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
static int process_all_heredocs_before_pipeline(t_cmd *cmds, t_shell *shell)
{
    t_cmd *current = cmds;
    
    while (current) 
    {
        if (current->heredoc && current->heredoc->enabled) 
        {
            // Heredoc'u işle ve input hazırla
            int hfd = multiple_heredoc_input(
                current->heredoc->delimiters,
                current->heredoc->cleaned_delimiters,
                current->heredoc->quoted_flags,
                current->heredoc->count,
                shell
            );
            
            if (hfd < 0) 
            {
                // Heredoc hatası
                return 0;
            }
            
            // Heredoc input'unu geçici dosyaya yaz
            char temp_filename[64];
            snprintf(temp_filename, sizeof(temp_filename), "/tmp/heredoc_%d_%p", getpid(), (void*)current);
            
            // Heredoc content'ini geçici dosyaya kopyala
            int temp_fd = open(temp_filename, O_WRONLY | O_CREAT | O_TRUNC, 0600);
            if (temp_fd >= 0) {
                char buffer[1024];
                ssize_t bytes_read;
                
                // Heredoc content'ini geçici dosyaya kopyala
                lseek(hfd, 0, SEEK_SET); // Dosya pointer'ı başa al
                while ((bytes_read = read(hfd, buffer, sizeof(buffer))) > 0) {
                    write(temp_fd, buffer, bytes_read);
                }
                close(temp_fd);
                close(hfd);
                
                // Geçici dosya adını heredoc yapısında sakla
                if (current->heredoc->temp_filename) {
                    free(current->heredoc->temp_filename);
                }
                current->heredoc->temp_filename = ft_strdup(temp_filename);
            }
        }
        current = current->next;
    }
    
    return 1; // Başarılı
}

// Geçici heredoc dosyalarını temizle
static void cleanup_heredoc_temp_files(t_cmd *cmds)
{
    t_cmd *current = cmds;
    
    while (current) {
        if (current->heredoc && current->heredoc->enabled && current->heredoc->temp_filename) {
            // Geçici dosyayı sil
            unlink(current->heredoc->temp_filename);
            
            // Belleği temizle
            free(current->heredoc->temp_filename);
            current->heredoc->temp_filename = NULL;
        }
        current = current->next;
    }
}

// 2. Dup2 işlemi - stdout/stderr/stdin yönlendir
static void setup_redirections(t_cmd *cmd, int fd_in, int *pipefd)
{
    // Heredoc yönlendirmesi - zaten pipeline öncesi işlendi
    if (cmd->heredoc && cmd->heredoc->enabled) {
        // Heredoc zaten pipeline öncesi işlendi
        // Sadece hazır input'u kullan
        if (cmd->heredoc->temp_filename) {
            int hfd = open(cmd->heredoc->temp_filename, O_RDONLY);
            if (hfd >= 0) {
                dup2(hfd, 0);
                close(hfd);
            }
        }
    }
    // Input dosya yönlendirmesi
    else if (cmd->infile) 
    {
        // Input dosyası var mı kontrol et
        if (access(cmd->infile, F_OK) != 0) {
            ft_putstr_fd(cmd->infile, 2);
            ft_putstr_fd(": Böyle bir dosya ya da dizin yok\n", 2);
            _exit(1);
        }
        
        int in = open(cmd->infile, O_RDONLY);
        if (in < 0) 
        { 
            _exit(1); // Error already handled by parent/builtin redirection
        }
        dup2(in, 0); 
        close(in);
    }
    // Pipe input yönlendirmesi
    else if (fd_in != 0) 
    { 
        dup2(fd_in, 0); 
        close(fd_in); 
    }
    
    // Output dosya yönlendirmesi
    if (cmd->outfile) 
    {
        int out = open(cmd->outfile, cmd->append ? 
            O_WRONLY|O_CREAT|O_APPEND : O_WRONLY|O_CREAT|O_TRUNC, 0644);
        if (out < 0) 
        { 
            perror(cmd->outfile); 
            _exit(1); 
        }
        dup2(out, 1); 
        close(out);
    }
    // Pipe output yönlendirmesi
    else if (cmd->next) 
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
        _exit(127); 
    }
    
    // Path contains slash - check if file exists
    if (strchr(cmd->argv[0], '/')) {
        if (access(path, F_OK) != 0) {
            ft_putstr_fd(cmd->argv[0], 2);
            ft_putstr_fd(": No such file or directory\n", 2);
            _exit(127);
        }
    }
    
    // Directory check
    struct stat st;
    if (stat(path, &st) == 0 && S_ISDIR(st.st_mode)) {
        ft_putstr_fd(cmd->argv[0], 2);
        ft_putstr_fd(": Is a directory\n", 2);
        _exit(126);
    }
    
    // Dosya var ama çalıştırılabilir değilse 126 exit code'u ver
    if (access(path, X_OK) != 0) {
        ft_putstr_fd(cmd->argv[0], 2);
        ft_putstr_fd(": Permission denied\n", 2);
        _exit(126);
    }
    
    execve(path, cmd->argv, envp);
    perror("execve"); 
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
    
    // Eğer komut yoksa (sadece heredoc/redirection varsa), cat gibi davran
    if (!cmd->argv || !cmd->argv[0]) {
        // Stdin'den stdout'a kopyala (cat benzeri davranış)
        char buffer[1024];
        ssize_t bytes_read;
        while ((bytes_read = read(0, buffer, sizeof(buffer))) > 0) {
            write(1, buffer, bytes_read);
        }
        exit(0);
    }
    
    // Önce builtin komut kontrolü yap
    if (is_builtin(cmd->argv[0])) {
        int result = exec_builtin(cmd, shell);
        // Builtin komutlar için normal exit kullan
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
                                     int *hfd, int *outfd)
{
    int error_count = 0;
    
    // Input file redirection kontrolü
    if (cmd->infile && !(cmd->heredoc && cmd->heredoc->enabled)) {
        *old_stdin = dup(0);
        
        // Input dosyası var mı kontrol et
        if (access(cmd->infile, F_OK) != 0) {
            ft_putstr_fd(cmd->infile, 2);
            ft_putstr_fd(": Böyle bir dosya ya da dizin yok\n", 2);
            error_count++;
        } else {
            int infd = open(cmd->infile, O_RDONLY);
            if (infd < 0) {
                error_count++;
            } else {
                dup2(infd, 0);
                close(infd);
            }
        }
    }
    
    // Heredoc yönlendirmesi
    if (cmd->heredoc && cmd->heredoc->enabled) {
        *old_stdin = dup(0);
        
        // Heredoc zaten pipeline öncesi işlendi
        // Sadece hazır input'u kullan
        if (cmd->heredoc->temp_filename) {
            *hfd = open(cmd->heredoc->temp_filename, O_RDONLY);
            if (*hfd >= 0) {
                dup2(*hfd, 0);
                close(*hfd);
            }
        }
    }
    
    // Output dosya yönlendirmesi
    if (cmd->outfile) {
        *old_stdout = dup(1);
        *outfd = open(cmd->outfile, cmd->append ? 
            O_WRONLY|O_CREAT|O_APPEND : O_WRONLY|O_CREAT|O_TRUNC, 0644);
        if (*outfd < 0) { 
            error_count++;
        } else { 
            dup2(*outfd, 1); 
            close(*outfd); 
        }
    }
    
    return error_count;
}

// Builtin komutlar için redirection geri yükleme
static void restore_builtin_redirections(t_cmd *cmd, int old_stdin, int old_stdout)
{
    if (cmd->outfile && old_stdout != -1) {
        dup2(old_stdout, 1);
        close(old_stdout);
    }
    if (cmd->heredoc && cmd->heredoc->enabled && old_stdin != -1) {
        dup2(old_stdin, 0);
        close(old_stdin);
    }
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
    if (!process_all_heredocs_before_pipeline(cmds, shell)) {
        setup_normal_signals(); // Normal sinyallere dön
        return 1; // Heredoc hatası
    }
    
    // Önce tüm command'larda syntax error kontrolü yap
    cmd = cmds;
    while (cmd) 
    {
        // Syntax error kontrolü - artık NULL argv'yi kabul ediyoruz (sadece heredoc/redirection için)
        cmd = cmd->next;
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
            run_child_process(cmd, envp, fd_in, pipefd, shell);
        } 
        else if (pid > 0) 
        {
            // Parent process
            if (fd_in != 0)  // önceki komutun  okuma ucunu kapa
                close(fd_in);
            if (cmd->next) 
            { 
                close(pipefd[1]); // yeni pipe ın yazma ucunu kapa
                fd_in = pipefd[0]; // sonraki pipe ın okuma ucu olarak kullanılacak bu yüzden sakla
            }
        }
        
        cmd = cmd->next;
    }
    
    // 4. Wait işlemi
    int result = wait_for_children();
    
    // Geçici heredoc dosyalarını temizle
    cleanup_heredoc_temp_files(cmds);
    
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
    int hfd = -1, outfd = -1;
    
    // Komut çalışmaya başladı - sinyal handler'ları güncelle
    setup_command_signals();
    
    // Redirections ayarla ve hata kontrolü yap
    int redir_errors = setup_builtin_redirections(cmd, &old_stdin, &old_stdout, &hfd, &outfd);
    

    
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