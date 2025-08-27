#include "../include/minishell.h"
#include <sys/types.h>
#include <sys/wait.h>
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
int	handle_normal_command(t_cmd *cmds, t_shell *shell)
{
	// Yeni mantık: Önce heredoc'ları işle, sonra komut türüne göre çalıştır
	return execute_command_main(cmds, shell);
}

int	determine_command_type(t_cmd *cmds, t_shell *shell)
{
	// Tüm komut türleri (builtin, variable assignment, normal command, redirection-only)
	// handle_normal_command içinde işlenecek
	return (handle_normal_command(cmds, shell));
}
