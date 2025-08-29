#include "../include/minishell.h"
#include <sys/types.h>
#include <sys/wait.h>


int execute_command_main(t_cmd *cmds, t_shell *shell)
{

    if (handle_all_heredocs(cmds, shell) == -1) 
        return -1;
    if (cmds->next){
        int result = exec_pipeline(cmds, shell);
        shell->last_exit = result;
        if (result == 0)
            return 1;
        return 0;
    }
    else if (cmds->argv && cmds->argv[0] && is_builtin(cmds->argv[0])) {
        int result = execute_single_builtin(cmds, shell);
        shell->last_exit = result;
        update_last_arg(cmds->argv, shell);
        if (result == 0) 
            return 1;
        
        return 0;
    }
    else 
    {
        int result = execute_single_external(cmds, shell);
        shell->last_exit = result;
        if (cmds->argv) {
            update_last_arg(cmds->argv, shell);
        }
        if (result == 0)
            return 1;
        return 0;    
    }
} 


int handle_all_heredocs(t_cmd *cmds, t_shell *shell)
{
    t_cmd *current = cmds;
    
    while (current) 
    {
        if (current->heredocs) 
        {
            t_heredoc *heredoc = current->heredocs;
            while (heredoc) 
            {
                int result = process_single_heredoc(heredoc, shell);
                if (result != 0) 
                {
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