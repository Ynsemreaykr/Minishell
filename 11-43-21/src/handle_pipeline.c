#include "../include/minishell.h"
#include <sys/unistd.h>
#include <fcntl.h>


// Pipeline öncesi tüm heredoc'ları işle
int first_heredoc_process(t_cmd *cmds, t_shell *shell)
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

// Pipeline execution - Ana execute fonksiyonu
int exec_pipeline(t_cmd *cmds, t_shell *shell)
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
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    
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
            run_child_process(cmd, fd_in, pipefd, shell); // child lar komutu çalıştırır
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


