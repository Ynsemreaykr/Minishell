#include "../include/minishell.h"

// Heredoc content'lerini temizle
void cleanup_heredoc_content(t_cmd *cmds)
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

// Environment variable'ları temizle
void cleanup_env(t_shell *shell)
{
    if (shell->env) {
        int i = 0;
        while (shell->env[i]) {
            ft_free(shell->env[i]);
            i++;
        }
        ft_free(shell->env);
        shell->env = NULL;
    }
}

// Child process için shell cleanup
void cleanup_shell_for_child(t_shell *shell)
{
    cleanup_env(shell);
    if (shell->last_arg) {
        ft_free(shell->last_arg);
        shell->last_arg = NULL;
    }
}

// Clean up resources and return NULL on syntax error
void cleanup_and_return_null(t_cmd *cmd, char **cmd_strings, int cmd_count)
{
    if (cmd) {
        // Önce cmd->argv içindeki token'ları free et
        if (cmd->argv) {
            int i = 0;
            while (cmd->argv[i]) {
                ft_free(cmd->argv[i]);
                i++;
            }
            ft_free(cmd->argv);
        }
        
        // Heredoc temizliği
        if (cmd->heredocs) {
            free_heredoc_list(cmd->heredocs);
        }
        
        // Redirection temizliği
        if (cmd->redirs) {
            free_redir_list(cmd->redirs);
        }
        
        ft_free(cmd);
    }
    // Tüm command'ları temizle
    int k=0;
    while(k < cmd_count) {
        ft_free(cmd_strings[k]);
        k++;
    }
    ft_free(cmd_strings);
}