#include "../include/minishell.h"
#include <sys/wait.h>
#include <sys/stat.h>
    

// Child process'in ana fonksiyonu
void run_child_process(t_cmd *cmd, int fd_in, int *pipefd, t_shell *shell)
{
    // Signal handler'ları sıfırla
    //signal(SIGINT, SIG_DFL);
    //signal(SIGQUIT, SIG_DFL);
    
    // Redirections'ları ayarla
    setup_redirections_for_child(cmd, fd_in, pipefd);
    
    // Eğer komut yoksa (sadece heredoc/redirection varsa), hiçbir çıktı verme
    if (!cmd->argv || !cmd->argv[0]) {
        // Sadece heredoc/redirection varsa, hiçbir çıktı verme
        setup_command_signals();
        cleanup_shell_for_child(shell);
        ft_mem_cleanup(); // Memory temizle
        exit(0);
    }

    // Child process'te signal handler'ları kur
    setup_command_signals();
    
    // Builtin değilse exec ile çalıştır
    execute_command(cmd, shell->env);
}

// 4. Wait işlemi - Parent, child'ları bekler
int wait_for_children(void)
{
    int status;
    pid_t last_pid = -1;
    int last_status = 0;
    
    // Tüm child process'leri bekle
    while ((last_pid = wait(&status)) > 0) {
        // Son child process'in status'unu sakla
        last_status = status;
    }
    
    // Signal ile sonlanan process'ler için özel handling
    if (WIFSIGNALED(last_status)) {
        int sig = WTERMSIG(last_status);
        if (sig == SIGINT) {
            // Ctrl+C için yeni satıra geç (son_shell gibi)
            write(STDOUT_FILENO, "\n", 1);
            return 130; // Ctrl+C için exit code 130
        } else if (sig == SIGQUIT) {
            return 131; // Ctrl+\ için exit code 131
        }
        return 128 + sig; // Diğer signaller için 128 + signal number
    }
    
    return last_status;
}

// 1. Fork işlemi - Yeni process yarat
pid_t create_child_process(void)
{
    pid_t pid = fork();
    if (pid < 0) 
    {
        perror("fork");
        return -1;
    }
    return pid;
}


// 3. Execve işlemi - Komutu çalıştır
void execute_command(t_cmd *cmd, char **envp)
{
    fprintf(stderr,"execute command: %s aa\n",cmd->argv[0]);

    char *path = find_path(cmd->argv[0], envp);
    if (!path) { 
        ft_putstr_fd(cmd->argv[0], 2);
        ft_putstr_fd(": command not found\n", 2); 
        ft_mem_cleanup(); // Child process'te memory temizle
        exit(127); 
    }
    
    // Path contains slash - check if file exists
    if (strchr(cmd->argv[0], '/')) {
        if (access(path, F_OK) != 0) {
            ft_putstr_fd(cmd->argv[0], 2);
            ft_putstr_fd(": No such file or directory\n", 2);
            ft_mem_cleanup(); // Child process'te memory temizle
            exit(127);
        }
    }
     // Directory check
    struct stat st;
    if (stat(path, &st) == 0 && S_ISDIR(st.st_mode)) {
        ft_putstr_fd(cmd->argv[0], 2);
        ft_putstr_fd(": Is a directory\n", 2);
        ft_mem_cleanup(); // Child process'te memory temizle
        exit(126);
    }
    
    // Dosya var ama çalıştırılabilir değilse 126 exit code'u ver
    if (access(path, X_OK) != 0) {
        ft_putstr_fd(cmd->argv[0], 2);
        ft_putstr_fd(": Permission denied\n", 2);
        ft_mem_cleanup(); // Child process'te memory temizle
        exit(126);
    }
    execve(path, cmd->argv, envp);
    perror("execve"); 
    ft_mem_cleanup(); // Execve başarısız olduğunda memory temizle
    exit(127);
}