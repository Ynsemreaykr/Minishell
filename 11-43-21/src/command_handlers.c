#include "../include/minishell.h"
#include <sys/types.h>
#include <sys/wait.h>

// Pipeline komutlarını işle
int handle_pipeline(t_cmd *cmds, t_shell *shell)
{
    int status = exec_pipeline(cmds, shell->envp, shell);
    
    if (status == 1) {
        // Heredoc hatası (Ctrl+C)
        shell->last_exit = 130; // Ctrl+C için exit code 130
        return 0; // Hata durumu
    } else if (WIFEXITED(status)) {
        shell->last_exit = WEXITSTATUS(status);
    }
    
    // Pipeline'da son komutun argümanlarını güncelle
    t_cmd *last_cmd = cmds;
    while (last_cmd->next) last_cmd = last_cmd->next;
    if (last_cmd->argv) // NULL kontrolü ekle
        update_last_arg(last_cmd->argv, shell);
    
    return 1; // Success
}

// Variable assignment komutlarını işle
int handle_variable_assignment(t_cmd *cmds, t_shell *shell)
{
    shell->last_exit = execute_variable_assignment(cmds->argv, shell);
    extern char **environ;
    shell->envp = environ; // Environment güncellendiği için environ'ı güncelle
    return 1; // Success
}

// Builtin komutları işle
int handle_builtin(t_cmd *cmds, t_shell *shell)
{
    shell->last_exit = exec_builtin_with_redirections(cmds, shell);
    update_last_arg(cmds->argv, shell);
    
    if (ft_strcmp(cmds->argv[0], "export") == 0) {
        extern char **environ;
        shell->envp = environ;
    }
    return 1; // Success
}

// Normal komutları işle (heredoc/redirection veya exec)
int handle_normal_command(t_cmd *cmds, t_shell *shell)
{
    int status = exec_pipeline(cmds, shell->envp, shell);
    
    if (status == 1) {
        // Heredoc hatası (Ctrl+C)
        shell->last_exit = 130; // Ctrl+C için exit code 130
        return 0; // Hata durumu
    } else if (WIFEXITED(status)) {
        shell->last_exit = WEXITSTATUS(status);
    }
    
    if (cmds->argv)
        update_last_arg(cmds->argv, shell);
    
    return 1; // Success
}

// Komut türünü belirle ve uygun handler'ı çağır
int determine_command_type(t_cmd *cmds, t_shell *shell)
{
    // Pipeline kontrolü - birden fazla komut varsa
    if (cmds->next) {
        return handle_pipeline(cmds, shell);
    }
    
    // Variable ataması  $VAR = yunus
    if (cmds->argv && cmds->argv[0] && is_variable_assignment(cmds->argv)) {
        return handle_variable_assignment(cmds, shell);
    }
    
    // Builtin kontrolü
    if (cmds->argv && cmds->argv[0] && is_builtin(cmds->argv[0])) {
        return handle_builtin(cmds, shell);
    }
    
    // Sadece redirection + dosya adı durumu (örn: > file, >> file)
    if (!cmds->argv && cmds->redirs) {
        return handle_builtin(cmds, shell);
    }
    
    // Normal komut (heredoc/redirection veya exec)
    return handle_normal_command(cmds, shell);
}
