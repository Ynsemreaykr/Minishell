/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkayaalp <mkayaalp@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 15:54:07 by mkayaalp          #+#    #+#             */
/*   Updated: 2025/09/10 10:33:08 by mkayaalp         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdlib.h>
# include <fcntl.h>
# include <signal.h>

typedef struct s_mem_block
{
	void				*ptr;
	struct s_mem_block	*next;
}	t_mem_block;

typedef enum e_redir_type
{
	REDIR_IN,
	REDIR_OUT,
	REDIR_APPEND,
	HEREDOC
}	t_redir_type;

typedef struct s_redir
{
	t_redir_type		type;
	char				*filename;
	char				*delimiter;
	char				*cleaned_delimiter;
	char				*content;
	int					quoted_flag;
	struct s_redir		*next;
}	t_redir;

typedef struct s_cmd
{
	char				**argv;
	t_redir				*redirs;
	struct s_cmd		*next;
}	t_cmd;

typedef struct s_shell
{
	t_cmd				*cmds;
	int					last_exit;
	char				**env;
}	t_shell;

typedef struct s_cmd_data
{
	t_cmd				*cmd;
	char				**cmd_strings;
	int					cmd_count;
	t_shell				*shell;
}	t_cmd_data;

typedef struct s_proc_ctx
{
	const char			*input;
	int					*i;
	int					end;
	char				*processed;
	int					*proc_len;
	t_shell				*shell;
}	t_proc_ctx;

int		ft_atoi(const char *str);
int		ft_isalnum(int c);
int		ft_isalpha(int c);
char	*ft_itoa(int n);
void	*ft_memset(void *s, int c, size_t n);
void	ft_putstr_fd(const char *str, int fd);
char	**ft_split(const char *s, char c);
void	ft_split_free(char **split_result);
char	*ft_strcat(char *dest, const char *src);
char	*ft_strchr(const char *s, int c);
int		ft_strcmp(const char *s1, const char *s2);
char	*ft_strcpy(char *dest, const char *src);
char	*ft_strdup(const char *s);
char	*ft_strjoin(const char *s1, const char *s2);
size_t	ft_strlen(const char *s);
int		ft_strncmp(const char *s1, const char *s2, size_t n);
char	*ft_strncpy(char *dest, const char *src, size_t n);

char	*process_single_quote(t_proc_ctx *ctx);

int		handle_heredoc_redir_builtin(t_redir *r, int *input_redirected);
int		redir_in_builtin(t_redir *r, int *input_redirected);
int		redir_out_builtin(t_redir *r, int *output_redirected, int *outfd);
int		redir_append_builtin(t_redir *r, int *output_redirected, int *outfd);

void	apply_heredoc_child(t_redir *r, int *input_redirected);
void	apply_out_append_child(t_redir *r);
void	apply_out_trunc_child(t_redir *r);
void	apply_in_redir_child(t_redir *r, int *input_redirected);

char	**get_env(t_shell *shell);
int		ft_env(t_shell *shell);
int		ft_cd(char **argv, t_shell *shell);

int		is_builtin(const char *cmd);
int		ft_exit(char **argv);
int		ft_pwd(t_shell *shell);

int		calculate_required_memory(const char *input,
			int start, int end, t_shell *shell);

pid_t	create_child_process(void);

void	cleanup_heredoc_content(t_cmd *cmds);
void	cleanup_env(t_shell *shell);
void	cleanup_shell_for_child(t_shell *shell);
void	cleanup_and_return_null(t_cmd *cmd, char **cmd_strings, int cmd_count);

int		process_cmd_loop(t_cmd_data *data, t_cmd **head, t_cmd **last);

int		execute_single_external(t_cmd *cmd, t_shell *shell);
int		execute_single_builtin(t_cmd *cmd, t_shell *shell);

int		execute_command_main(t_cmd *cmds, t_shell *shell);

int		get_redirection_length(const char *input, int pos);
int		check_redirection_operator(const char *input, int pos);
void	update_quote_state(const char *input, int *pos,
			int *in_quote, char *quote_char);
int		count_tokens(const char *input);
int		skip_whitespace(const char *input, int pos);

int		count_quoted_token(const char *input, int *i);
int		count_redirection_token(const char *input, int *i);
int		process_regular_token(const char *input,
			int *pos, int *in_quote, char *quote_char);

void	add_redir(t_cmd *cmd, t_redir *redir);
t_redir	*create_redir(t_redir_type type, char *filename);
t_redir	*create_heredoc_redir(char *delimiter,
			char *cleaned_delimiter, int quoted_flag);
void	process_output_redir(t_cmd *cmd, int *i);
void	handle_in_redir(t_cmd *cmd, char **argv, int *i);
int		is_valid_redir_target(const char *token);

int		set_env_var(const char *name, const char *value, t_shell *shell);

void	init_env(char **envp, t_shell *shell);
char	*get_env_var(const char *name, t_shell *shell);
void	update_shlvl(t_shell *shell);

int		ft_echo(char **argv);
int		exec_builtin(t_cmd *cmd, t_shell *shell);
int		ft_dot(char **argv);
int		exec_pipeline(t_cmd *cmds, t_shell *shell);
int		is_builtin_command(const char *cmd);

void	execute_command(t_cmd *cmd, char **envp);

void	setup_redirections_for_child(t_cmd *cmd, int fd_in, int *pipefd);

int		expand_environment_variable(const char *line,
			int start, t_shell *shell, char *expanded);

int		calculate_expansion_size(const char *line, t_shell *shell);

int		handle_no_args_export(t_shell *shell);

int		ft_export(char **argv, t_shell *shell);
char	*extract_var_name(const char *arg);
int		is_valid_identifier(const char *name);

int		unset_env_var(const char *name, t_shell *shell);
int		ft_unset(char **argv, t_shell *shell);

void	heredoc_child_process(int *pipefd,
			t_redir *heredoc_redir, t_shell *shell);

void	process_expansion_loop(const char *line,
			t_shell *shell, char *expanded);

int		detect_quote_clean_delimiter(char **delimiter);

int		process_single_heredoc(t_redir *heredoc_redir, t_shell *shell);

int		check_redir_operator(const char *input, int *i);

int		check_input(const char *input, t_shell *shell);

int		process_command_from_input(const char *input, t_shell *shell);
void	cleanup_shell(t_shell *shell);

void	*ft_malloc(size_t size);
void	ft_free(void *ptr);
void	ft_mem_cleanup(void);

t_cmd	*parse_commands(const char *input, t_shell *shell);

void	parse_redirections_and_heredoc(t_cmd *cmd);

char	**parse_path(char **envp);
int		search_in_path(char *command, char **splitted_path, char **full_path);

int		count_pipes(const char *input);
int		validate_input_and_pipes(const char *input);

void	toggle_quote_state(char c,
			int *in_single_quote, int *in_double_quote);
char	**split_by_pipes(const char *input, int *count);

void	process_exit_status(int code, char *processed, int *proc_len, int *i);
void	process_literal_dollar(char *processed, int *proc_len, int *i);
void	process_regular_char(const char *input,
			int *i, char *processed, int *proc_len);
void	process_variable_expansion_in_quotes(t_proc_ctx *ctx);

void	process_variable_expansion_outside_quotes(t_proc_ctx *ctx);

char	*read_heredoc_content(int fd);
char	*read_line_dynamic(int fd);

int		setup_redirections_for_builtin(t_cmd *cmd);
int		wait_for_pipeline(pid_t last_pid);

void	normal_signal_handler(int signo);
void	heredoc_signal_handler(int signo);
void	command_signal_handler(int signo);
void	handle_sigquit(int signo);

void	handle_sigpipe(int signo);
void	setup_normal_signals(void);
void	setup_heredoc_signals(void);
void	setup_command_signals(void);
void	reset_signal_state(void);

void	print_sorted_env(char **env, int count);
void	sort_env(char **env, int count);

void	parse_heredoc_delimiter(const char *input,
			int *i, char **args, int *argc);

char	*process_token_content(const char *input,
			int start, int end, t_shell *shell);

char	*process_regular_quoted_token(const char *input,
			int *i, t_shell *shell);

char	**split_tokens(const char *input, t_shell *shell);

void	free_args(char **args);
void	free_cmds(t_cmd *cmd);
void	free_redir_list(t_redir *redirs);

#endif
