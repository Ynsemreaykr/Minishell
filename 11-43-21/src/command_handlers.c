#include "../include/minishell.h"
#include <sys/types.h>
#include <sys/wait.h>

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

	printf("hhhhhh\n");
	int	result;

	result = exec_builtin_with_redirections(cmds, shell);
	shell->last_exit = result;
	update_last_arg(cmds->argv, shell);
	if (result == 0)
		return (1);
	else
		return (0);
}

int	handle_normal_command(t_cmd *cmds, t_shell *shell)
{
	int		result;
	int		status;
	t_cmd	*last_cmd;

	// Eğer komut pipe'sız ve heredoc varsa, önce heredoc'u işle
	if (!cmds->next && cmds->heredocs) {
		if (!first_heredoc_process(cmds, shell)) {
			shell->last_exit = 1;
			return (0);
		}
	}

	// Variable assignment kontrolü
	if (cmds->argv && cmds->argv[0] && is_variable_assignment(cmds->argv))
	{
		result = handle_variable_assignment(cmds, shell);
		if (result == 0)
			return (0);
		return (1);
	}
	
	// Builtin komut kontrolü
	if (cmds->argv && cmds->argv[0] && is_builtin(cmds->argv[0]))
	{
		result = handle_builtin(cmds, shell);
		if (result == 0)
			return (0);
		return (1);
	}
	
	// Sadece redirection olan komut kontrolü
	if (!cmds->argv && cmds->redirs)
	{
		result = handle_builtin(cmds, shell);
		if (result == 0)
			return (0);
		return (1);
	}

	// Normal komut (external command) çalıştırma
	status = exec_pipeline(cmds, shell);
	if (status == 1)
	{
		shell->last_exit = 130;
		return (0);
	}
	else if (WIFEXITED(status)) 
		shell->last_exit = WEXITSTATUS(status);
	last_cmd = cmds;
	while (last_cmd->next)
		last_cmd = last_cmd->next;
	if (last_cmd->argv)
		update_last_arg(last_cmd->argv, shell);
	return (1);
}

int	determine_command_type(t_cmd *cmds, t_shell *shell)
{
	return (handle_normal_command(cmds, shell));
}
