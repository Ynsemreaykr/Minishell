#include <signal.h>
#include <stdio.h>
#include <readline/readline.h>
#include <unistd.h>
#include <sys/wait.h>
#include "../include/minishell.h"

// Global sinyal numarası
extern volatile sig_atomic_t g_signal_number;

// Normal durum sinyal handler'ı (prompt'ta)
void normal_signal_handler(int signo)
{
    g_signal_number = signo;
    
    // Önce yeni satıra geç
    write(STDOUT_FILENO, "\n", 1);
    
    // Satırı tamamen temizle
    rl_replace_line("", 0);
    
    // Yeni satıra geç
    rl_on_new_line();
    
    // Prompt'u yeniden göster
    rl_redisplay();
}

// Heredoc sinyal handler'ı
void heredoc_signal_handler(int signo)
{
    g_signal_number = signo;
    write(STDOUT_FILENO, "\n", 1);
    rl_done = 1;
    rl_point = rl_end = 0;
    rl_replace_line("", 0);
}

// Komut çalışırken sinyal handler'ı
void command_signal_handler(int signo)
{
    g_signal_number = signo;
    //rl_on_new_line();

    //write(STDOUT_FILENO, "\n", 1);
    // Komut sinyal ile sonlandırıldı
}

// SIGQUIT handler'ı
void handle_sigquit(int signo)
{
    g_signal_number = signo;
    // SIGQUIT için özel işlem yapılmaz, sadece kaydedilir
}

// Normal sinyalleri kur (prompt'ta)
void setup_normal_signals(void)
{
    signal(SIGINT, normal_signal_handler);
    signal(SIGQUIT, handle_sigquit);
}

// Heredoc sinyallerini kur
void setup_heredoc_signals(void)
{
    signal(SIGINT, heredoc_signal_handler);
    signal(SIGQUIT, handle_sigquit);
}

// Komut sinyallerini kur
void setup_command_signals(void)
{
    signal(SIGINT, command_signal_handler);
    signal(SIGQUIT, handle_sigquit);
}

// Sinyal durumunu sıfırla
void reset_signal_state(void)
{
    g_signal_number = 0;
}

// Sinyal durumunu kontrol et ve gerekli exit code'u döndür
int check_signal_status(void)
{
    if (g_signal_number == SIGINT) {
        return 130; // SIGINT için exit code
    } else if (g_signal_number == SIGQUIT) {
        return 131; // SIGQUIT için exit code
    }
    return 0; // Sinyal yok
} 