#ifndef MINISHELL_H
#define MINISHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/rlconf.h>
#include <stdarg.h>

// Global olarak sadece sinyal numarası
extern volatile sig_atomic_t g_signal_number;

// Redirection türleri
typedef enum e_redir_type
{
    REDIR_IN,           // <
    REDIR_OUT,          // >
    REDIR_APPEND,       // >>
}   t_redir_type;

// Tek bir redirection bilgisi
typedef struct s_redir
{
    t_redir_type    type;           // Redirection türü
    char            *filename;      // Dosya adı
    struct s_redir  *next;          // Sonraki redirection
}   t_redir;

// Tek bir heredoc bilgisi
typedef struct s_heredoc
{
    char    *delimiter;                 // orijinal delimiter (tırnaklı)
    char    *cleaned_delimiter;         // temizlenmiş delimiter (tırnaksız)
    int     quoted_flag;                // quoted bayrağı
    char    *content;                   // heredoc içeriği
    struct s_heredoc *next;             // sonraki heredoc
}   t_heredoc;

    // Komut ve argümanları tutan yapı
typedef struct s_cmd
{
    char    **argv;         // Komut ve argümanlar
    t_redir *redirs;        // Redirection listesi (sıralı)
    t_heredoc *heredocs;    // heredoc listesi (sıralı)
    struct s_cmd *next;     // Pipe ile bağlı bir sonraki komut
}   t_cmd;

// Shell'in ana yapısı
typedef struct s_shell
{
    t_cmd   *cmds;          // Komutların bağlı listesi
    char    **envp;         // Ortam değişkenleri
    int     last_exit;      // Son komutun çıkış kodu ($?)
    char    *last_arg;      // Son komutun son argümanı
    char    **env;          // Environment variables
}   t_shell;



// Input result yapısı
typedef struct s_input_result
{
    char *input;             // Kullanıcı input'u
    int exit_code;           // Exit code
} t_input_result;


// Global sinyal numarası tanımı
int first_heredoc_process(t_cmd *cmds, t_shell *shell);
void normal_signal_handler(int signo);
void heredoc_signal_handler(int signo);
void command_signal_handler(int signo);
void handle_sigquit(int signo);
void setup_normal_signals(void);
void setup_heredoc_signals(void);
void setup_command_signals(void);
void reset_signal_state(void);
int check_signal_status(void);
int count_tokens(const char *input);
void cleanup_heredoc_content(t_cmd *cmds);
int handle_redir_append(t_redir *redir);
int handle_redir_out(t_redir *redir);
int handle_redir_in(t_redir *redir);
void setup_heredoc_stdin(t_cmd *cmd, int *input_redirected);
void setup_redir_list(t_cmd *cmd, int *input_redirected, int *output_redirected);       

int get_redirection_length(const char *input, int pos);
int check_redirection_operator(const char *input, int pos);
int skip_whitespace(const char *input, int pos);
void update_quote_state(const char *input, int *pos, int *in_quote, char *quote_char);
char *process_token_content(const char *input, int start, int end, int last_exit, t_shell *shell);
void process_escape_sequence(const char *input, int *i, char *processed, int *proc_len);
void process_exit_status(int last_exit, char *processed, int *proc_len);
void process_variable_expansion_in_quotes(const char *input, int *i, int end, char *processed, int *proc_len, t_shell *shell);
void process_variable_expansion_outside_quotes(const char *input, int *i, int end, char *processed, int *proc_len, t_shell *shell);
void process_literal_dollar(char *processed, int *proc_len, int *i);
void process_regular_char(const char *input, int *i, char *processed, int *proc_len);

void run_child_process(t_cmd *cmd, int fd_in, int *pipefd, t_shell *shell);
void setup_redirections_for_child(t_cmd *cmd, int fd_in, int *pipefd);
void execute_command(t_cmd *cmd, char **envp);
int wait_for_children(void);
pid_t create_child_process(void);

// LIBFT functions
size_t ft_strlen(const char *s);
char *ft_strcpy(char *dest, const char *src);
char *ft_strcat(char *dest, const char *src);
char *ft_strdup(const char *s);
int ft_strcmp(const char *s1, const char *s2);
int ft_strncmp(const char *s1, const char *s2, size_t n);
char *ft_strchr(const char *s, int c);
char *ft_strncpy(char *dest, const char *src, size_t n);
int ft_atoi(const char *str);
void *ft_calloc(size_t nmemb, size_t size);
void *ft_memset(void *s, int c, size_t n);
int ft_isalnum(int c);
int ft_isalpha(int c);
void ft_putstr(const char *str);
void ft_putstr_fd(const char *str, int fd);
char *ft_strtok(char *str, const char *delim);
char *ft_strtok_r(char *str, const char *delim, char **saveptr);
void expand_env_vars(const char *src, char *dst, int dstsize, char **envp, int last_exit);
char *ft_expand_env_vars(const char *src, char *dst, int last_exit, char **envp);
char *ft_expand_tilde(const char *str);
char *ft_itoa(int n);
char **ft_split(const char *s, char c);
void ft_split_free(char **split_result);



// Environment functions
void init_env(char **envp, t_shell *shell);
void cleanup_env(t_shell *shell);
char **get_env(t_shell *shell);
char *get_env_var(const char *name, t_shell *shell);
int set_env_var(const char *name, const char *value, t_shell *shell);
int unset_env_var(const char *name, t_shell *shell);
void update_shlvl(t_shell *shell);

// tokenizer_utils.c'den fonksiyonlar
extern int is_redirection_operator(const char *s, int i, int r_index);
extern int is_exit_status_var(const char *s, int i, int r_index);

// Quote handling için parametre struct'ı
typedef struct s_quote_context
{
    const char *s;           // Input string
    int *i;                  // Current position
    char *res;               // Result buffer
    int *r_index;            // Result index
    int *expand;             // Expansion flag -> genişletilecek mi
    int *unclosed_quote;     // Unclosed quote flag
} t_quote_context;

extern void handle_quotes(const char *s, int *i, char *res, int *r_index, int *flags);
extern char *handle_tilde_expansion(char *res);
extern void extract_variable_name(const char *token, int var_start, char *varname);
extern void expand_exit_status(char *buf, int *b, int last_exit);
extern void expand_last_arg(char *buf, int *b);
extern void expand_env_variable(char *buf, int *b, const char *varname, char **envp);
extern char **resize_args_array(char **args, int *argc, int split_count);

// parser.c
t_cmd *parse_commands(const char *input, int last_exit, t_shell *shell);
int is_variable_assignment(char **argv);
int execute_variable_assignment(char **argv, t_shell *shell);

// pipe_parser.c
char **split_by_pipes(const char *input, int *count);

// tokenizer.c
char **split_tokens(const char *input, int last_exit, t_shell *shell);

// heredoc.c
int multiple_heredoc_input(t_heredoc *heredoc, t_shell *shell);
char *read_line_dynamic(int fd);

// utils.c
void free_args(char **args);
void free_cmds(t_cmd *cmd);
char *find_path(char *cmd, char **envp);
char *optimize_path(char **envp);
void update_last_arg(char **argv, t_shell *shell);

// redirection utils
t_redir *create_redir(t_redir_type type, char *filename);
void add_redir(t_cmd *cmd, t_redir *redir);
void free_redir_list(t_redir *redirs);

// heredoc utils
t_heredoc *create_heredoc(char *delimiter, char *cleaned_delimiter, int quoted_flag);
void add_heredoc(t_cmd *cmd, t_heredoc *heredoc);
void free_heredoc_list(t_heredoc *heredocs);
int count_heredocs(t_heredoc *heredocs);

int check_quotes(const char *input);
int check_redirection_syntax(const char *input);
void	redirect_heredoc_for_builtin(t_cmd *cmd, int *old_stdin, int *input_redirected);
int  redir_append_builtin(t_redir *r, int *old_stdout, int *output_redirected, int *outfd);
int  redir_out_builtin(t_redir *r, int *old_stdout, int *output_redirected, int *outfd);
int  redir_in_builtin(t_redir *r, int *old_stdin, int *input_redirected);
// executor.c
int exec_pipeline(t_cmd *cmds, t_shell *shell);
int exec_builtin_with_redirections(t_cmd *cmd, t_shell *shell);
char *read_heredoc_content(int fd);
// command_handlers.c
int handle_variable_assignment(t_cmd *cmds, t_shell *shell);
int handle_builtin(t_cmd *cmds, t_shell *shell);
int handle_normal_command(t_cmd *cmds, t_shell *shell);
int determine_command_type(t_cmd *cmds, t_shell *shell);


int exec_single_command(t_cmd *cmd, t_shell *shell);

// builtin.c
int ft_echo(char **argv);
int ft_exit(char **argv);
int ft_pwd(char **argv);
int ft_env(char **envp, t_shell *shell);
int ft_cd(char **argv, t_shell *shell);
int ft_export(char **argv, t_shell *shell);
int ft_unset(char **argv, t_shell *shell);
int is_builtin(const char *cmd);
int exec_builtin(t_cmd *cmd, t_shell *shell);


// memory.c
void *ft_malloc(size_t size, const char *file, int line);
void ft_free(void *ptr);
void ft_mem_cleanup(void);
void ft_mem_print(void);

#endif // MINISHELL_H
