#include "../include/minishell.h"
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

// Global olarak sadece sinyal numarası
volatile sig_atomic_t g_signal_number = 0;

// 1. Shell'i başlat
static t_shell init_shell(char **envp)
{
    t_shell shell;
    shell.envp = envp;
    shell.last_exit = 0; // son komutun çıkış kodu
    shell.last_arg = NULL; // son argüman
    shell.env = NULL; // environment variables
    init_env(envp, &shell);
    update_shlvl(&shell); // SHLVL değişkenini güncelle
    setup_normal_signals(); // Normal durum sinyallerini kur
    return shell;
}



// 2. Kullanıcı input'unu al
static t_input_result get_user_input(void)
{
    t_input_result result;
    char *input = readline("minishell$ ");
    
    if (!input) {
        printf("exit\n");
        result.input = NULL;
        result.exit_code = 0;
        return result;
    }
    
    // Global sinyal durumunu kontrol et
    int signal_exit_code = check_signal_status();
    if (signal_exit_code > 0) {
        result.input = input;
        result.exit_code = signal_exit_code;
        reset_signal_state(); // Sinyal durumunu temizle
        return result;
    }
    
    if (*input) {
        add_history(input);
    }
    
    result.input = input;
    result.exit_code = 0;
    return result;
}

// Komut türünü belirle ve uygun handler'ı çağır
// Bu fonksiyon command_handlers.c dosyasında tanımlanmıştır

// 3. Komutu işle
static void process_command(t_cmd *cmds, t_shell *shell)
{
    // Herhangi bir komut var mı kontrol et (argv yoksa da heredoc olabilir)
    if (!cmds) {
        return;
    }
    
    determine_command_type(cmds, shell);
}

// Input validation yap
static int check_input(const char *input, t_shell *shell)
{
    // Check for empty input (only whitespace)
    int only_whitespace = 1;
    int i = 0;
    while (input[i]) 
    {
        if (input[i] != ' ' && input[i] != '\t') 
        {
            only_whitespace = 0;
            break;
        }
        i++;
    }
    
    if (only_whitespace) {
        return 0; // Empty input - continue
    }
    
    // Tırnak kontrolü ekle
    if (!check_quotes(input)) {
        shell->last_exit = 2; // Syntax error için exit code 2
        return 0; // Continue
    }
    
    // Redirection syntax kontrolü ekle
    if (!check_redirection_syntax(input)) {
        shell->last_exit = 2; // Syntax error için exit code 2
        return 0; // Continue
    }
    
    return 1; // Input valid
}

// Input'u handle et
static int handle_input(t_input_result input_result, t_shell *shell)
{
    char *input = input_result.input;
    
    if (!input) {
        return 0; // Exit
    }
    
    // Sinyal exit code'u varsa shell'e aktar
    if (input_result.exit_code > 0) {
        shell->last_exit = input_result.exit_code;
    }
    
    if (!ft_strcmp(input, "exit")) {
        ft_free(input);
        return 0; // Exit
    }
    
    // Input validation yap
    if (!check_input(input, shell)) {
        ft_free(input);
        return 1; // Continue
    }
    
    return 2; // Process command
}

// Komutu işle
static int process_command_from_input(const char *input, t_shell *shell)
{
    t_cmd *cmds = parse_commands(input, shell->last_exit, shell);
    
    // Parse error durumunda komutları işleme
    if (!cmds) {
        shell->last_exit = 2; // Syntax error için exit code 2
        return 0; // Error
    }
    
    // Eğer argv yok ama heredoc varsa (örn: <<aa durumu) bu normal
    if (cmds && !cmds->argv && !cmds->heredocs) {
        // Parse error durumunda sadece temizlik yap
        free_cmds(cmds);
        return 0; // Error
    }
    
    if (cmds && cmds->argv && cmds->argv[0] && ft_strlen(cmds->argv[0]) == 0) {
        // Boş string komut - command not found hatası ver
        ft_putstr_fd(": command not found\n", 2);
        shell->last_exit = 127; // Command not found exit code
        free_cmds(cmds);
        return 0; // Error
    }
    
    process_command(cmds, shell); // komutları çalıştırır
    free_cmds(cmds);
    return 1; // Success
}

// 4. Shell'i temizle
static void cleanup_shell(t_shell *shell)
{
    cleanup_env(shell);
    
    if (shell->last_arg) {
        ft_free(shell->last_arg);
        shell->last_arg = NULL;
    }
    
    // Global sinyal durumunu temizle
    reset_signal_state();
    
    rl_clear_history();
    ft_mem_cleanup();
}

// 5. Ana fonksiyon
int main(int argc, char **argv, char **envp)
{
    (void)argc;
    (void)argv;
    
    t_shell shell = init_shell(envp);
    while (1) {
        t_input_result input_result = get_user_input();
        int result = handle_input(input_result, &shell);
        
        if (result == 0) {
            break; // Exit
        } else if (result == 1) {
            continue; // Continue
        } else if (result == 2) {
            // Process command
            if (!process_command_from_input(input_result.input, &shell)) {
                ft_free(input_result.input);
                continue;
            }
            ft_free(input_result.input);
        }
    }
    cleanup_shell(&shell);
    
    return 0;
}
