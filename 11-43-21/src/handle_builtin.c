#include "../include/minishell.h"
#include <fcntl.h>

// Builtin komutlar için redirection geri yükleme
static void restore_builtin_redirections(int old_stdin, int old_stdout)
{
    // Redirection yapıldıysa geri yükle
    if (old_stdout != -1) {
        dup2(old_stdout, 1);
        close(old_stdout);
    }
    if (old_stdin != -1) {
        dup2(old_stdin, 0);
        close(old_stdin);
    }
}



static int	setup_builtin_redirections(t_cmd *cmd, int *old_stdin,
					int *old_stdout, int *outfd)
{
	int		error_count;
	int		input_redirected;
	int		output_redirected;
	t_redir	*current;

	error_count = 0;
	input_redirected = 0;
	output_redirected = 0;
	/* Heredoc yönlendirmesi */
	redirect_heredoc_for_builtin(cmd, old_stdin, &input_redirected);
	/* Redirection listesini işle (builtin için) */
	if (cmd->redirs)
	{
		current = cmd->redirs;
		while (current)
		{
			if (current->type == REDIR_IN)
				error_count += redir_in_builtin(current, old_stdin,
						&input_redirected);
			else if (current->type == REDIR_OUT)
				error_count += redir_out_builtin(current, old_stdout,
						&output_redirected, outfd);
			else if (current->type == REDIR_APPEND)
				error_count += redir_append_builtin(current, old_stdout,
						&output_redirected, outfd);
			current = current->next;
		}
	}
	return (error_count);
}

// Builtin komut execution
int exec_builtin_with_redirections(t_cmd *cmd, t_shell *shell)
{
    int old_stdin = -1;
    int old_stdout = -1;
    int outfd = -1;
    
    // Komut çalışmaya başladı - sinyal handler'ları güncelle
    setup_command_signals();
    
    // Redirections ayarla ve hata kontrolü yap
    int redir_errors = setup_builtin_redirections(cmd, &old_stdin, &old_stdout, &outfd);
    
    printf("redir error: %d\n",redir_errors);

    int result = 0;
    if (redir_errors > 0) {
        result = 1; // Redirection hatası varsa exit code 1
    } else {
        // Builtin komutu çalıştır
        printf("gggggggg\n");
        result = exec_builtin(cmd, shell);
    }
    
    // Redirections geri yükle
    restore_builtin_redirections(old_stdin, old_stdout);
    
    // Komut tamamlandı - normal sinyallere dön
    setup_normal_signals();
    
    return result;
} 



