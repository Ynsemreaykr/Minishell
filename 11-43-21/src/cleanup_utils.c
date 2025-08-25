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