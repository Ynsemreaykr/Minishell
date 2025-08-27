#include "../include/minishell.h"
#include <sys/unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>


// Pipeline öncesi tüm heredoc'ları işle
// Bu fonksiyon artık command_executors.c'de tanımlı

// Pipeline execution - Ana execute fonksiyonu
int exec_pipeline(t_cmd *cmds, t_shell *shell)
{
    fprintf(stderr,"exec pipeline\n");
    int fd_in = 0; // ilk komutun stdin i : terminal
    int pipefd[2]; // pipe[0] : okuma pipefd[1] : yazma ucu
    pid_t pid;
    t_cmd *cmd = cmds;
    
    // Komut çalışmaya başladı - sinyal handler'ları güncelle
    setup_command_signals();
    
     /*ÖNCE: Tüm komutların heredoc'larını işle
    if (handle_all_heredocs(cmds) == -1) {
        setup_normal_signals(); // Normal sinyallere dön
        return 1; // Heredoc hatası
    }*/
    
    cmd = cmds; 
    while (cmd) 
    {
        // Pipe oluştur (eğer sonraki komut varsa, son pipe a geldiysek açma)
        if (cmd->next) {
            pipe(pipefd);
        }
        
        // 1. Fork işlemi
        pid = create_child_process();
        
        if (pid == 0) 
        {
            // Child process
            // Sinyal handler'ları ayarla
            signal(SIGINT, SIG_DFL);
            signal(SIGQUIT, SIG_DFL);
            
            // Pipe setup'ı burada yap
            if (cmd->next) {
                // Sağ tarafa veri verme (pipe'a yaz)
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]);
                close(pipefd[0]); // Okuma ucunu kapat
            }
            
            // Sol taraftan veri alma (pipe'dan oku)
            if (fd_in != 0) {
                dup2(fd_in, STDIN_FILENO);
                close(fd_in);
            }
            
            // Redirections'ları ayarla (pipe setup'tan sonra)
            // Ama pipe setup'ını bozmasın
            setup_redirections_for_child(cmd, fd_in, pipefd);
            fprintf(stderr,"setup redir sonrası\n");    
            // Komutu çalıştır
            if (!cmd->argv || !cmd->argv[0]) {
                // Sadece heredoc/redirection varsa, hiçbir çıktı verme
                setup_command_signals();
                cleanup_shell_for_child(shell);
                ft_mem_cleanup();
                exit(0);
            }
            fprintf(stderr,"is builtin kontorlü öncesi\n");
            // Builtin değilse exec ile çalıştır
            fprintf(stderr,"is built sonuç: %s:     , %d\n",cmd->argv[0],is_builtin(cmd->argv[0]));
            if (is_builtin(cmd->argv[0])) {
                // Child process'te signal handler'ları kur
                setup_command_signals();
                exec_builtin(cmd, shell);
                cleanup_shell_for_child(shell);
                ft_mem_cleanup();
                exit(0);
            } else {
                execute_command(cmd, shell->env);
                // execute_command exec yapıyor, buraya asla gelmez
                // Ama güvenlik için cleanup ekle
                cleanup_shell_for_child(shell);
                ft_mem_cleanup();
                exit(127);
            }
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
    // Parent process'te sinyalleri ignore et
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    
    int result = wait_for_children();
    fprintf(stderr,"exec pipeline bitti\n");

    // Heredoc content'lerini temizle
    cleanup_heredoc_content(cmds);
    
    // Normal sinyallere dön
    setup_normal_signals();
    
    return result;
}


