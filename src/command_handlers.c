#include "../include/minishell.h"
#include <sys/types.h>
#include <sys/wait.h>

// Ana execute fonksiyonu
int execute_command_main(t_cmd *cmds, t_shell *shell)
{
    if (handle_all_heredocs(cmds) == -1) 
	{
		ft_mem_cleanup();
        return -1;
    }

    if (cmds->next) {
        return exec_pipeline(cmds, shell);
    } 
	else if (cmds->argv && cmds->argv[0] && is_builtin(cmds->argv[0])) 
	{
        int result = execute_single_builtin(cmds, shell);
        shell->last_exit = result;
        update_last_arg(cmds->argv, shell);
        return (result == 0 ? 1 : 0);
    } 
	else 
	{
        int result = execute_single_external(cmds, shell);
        shell->last_exit = result;
        
        if (cmds->argv) {
            update_last_arg(cmds->argv, shell);
        }
        return (result == 0 ? 1 : 0);
    }
} 


// Pipeline execution - Ana execute fonksiyonu
int exec_pipeline(t_cmd *cmds, t_shell *shell)
{
    int fd_in = 0; // ilk komutun stdin i : terminal
    int pipefd[2]; // pipe[0] : okuma pipefd[1] : yazma ucu
    pid_t pid;
    t_cmd *cmd = cmds;
    
    setup_command_signals();
    

    
    cmd = cmds; 
    while (cmd) 
    {
        if (cmd->next) {
            pipe(pipefd);
        }
        
        // 1. Fork işlemi
        pid = create_child_process();
        
        if (pid == 0) 
        {
           
            signal(SIGINT, SIG_DFL);
            signal(SIGQUIT, SIG_DFL);
            signal(SIGPIPE, SIG_IGN); // SIGPIPE'ı ignore et
            
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
            
            // Komutu çalıştır
            if (!cmd->argv || !cmd->argv[0]) {
                // Sadece heredoc/redirection varsa, hiçbir çıktı verme
                ft_mem_cleanup();
                exit(0);
            }
            
            // Builtin değilse exec ile çalıştır
            if (is_builtin(cmd->argv[0])) {
                exec_builtin(cmd, shell);
                ft_mem_cleanup();
                exit(0);
            } else {
                execute_command(cmd, shell->env);
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
    
    // Heredoc content'lerini temizle
    cleanup_heredoc_content(cmds);
    
    // Normal sinyallere dön
    setup_normal_signals();
    
    return result;
}

// Single builtin komutu çalıştır (parent process'te)
int execute_single_builtin(t_cmd *cmd, t_shell *shell)
{
    int old_stdin = dup(STDIN_FILENO);
    int old_stdout = dup(STDOUT_FILENO);
    int result = 0;
    

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
    
    // Heredoc content'lerini temizle
    cleanup_heredoc_content(cmd);
    
    return result;
}



/*
int	handle_variable_assignment(t_cmd *cmds, t_shell *shell)
{
	int	result;

	result = execute_variable_assignment(cmds->argv, shell);
	shell->last_exit = result;
	if (result == 0)
		return (1);
	else
		return (0);
}

int	handle_builtin(t_cmd *cmds, t_shell *shell)
{
	int	result;

	result = exec_builtin_with_redirections(cmds, shell);
	shell->last_exit = result;
	update_last_arg(cmds->argv, shell);
	if (result == 0)
		return (1);
	else
		return (0);
}
*/

