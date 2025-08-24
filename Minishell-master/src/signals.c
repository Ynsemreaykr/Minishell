#include <signal.h>
#include <stdio.h>
#include <readline/readline.h>
#include <unistd.h>
#include <sys/wait.h>
#include "../include/minishell.h"

// Global sinyal numarası
extern volatile sig_atomic_t g_signal_number;

// Debug fonksiyonu
void debug_print_signal(const char *context, int signo) {
    fprintf(stderr, "[DEBUG] Signal in %s: %d (SIGINT: %d, SIGQUIT: %d)\n", 
            context, signo, SIGINT, SIGQUIT);
    fflush(stderr);
}
/*
// Heredoc sinyal handler'ı
void heredoc_signal_handler(int signo)
{
    // Global sinyal numarasını ayarla
    g_signal_number = signo;
    
    // Yeni satıra geç
    write(STDOUT_FILENO, "\n", 1);
    
    // Readline'ı zorla sonlandır
    rl_done = 1;
    
    // Input'u temizle
    rl_replace_line("", 0);
    rl_on_new_line();
}
void heredoc_signal_handler(int signo)
{
    g_signal_number = signo;
    write(STDOUT_FILENO, "\n", 1);
    
    // Readline'ı interrupt etmek için - bunlar izin verilen
    rl_replace_line("", 0);  // Satırı temizle
    rl_on_new_line();        // Yeni satıra geç  
    rl_redisplay();          // Ekranı yenile
    rl_done = 1;             // Readline'dan çık
}*/
void heredoc_signal_handler(int signo)
{
    g_signal_number = signo;
    write(STDOUT_FILENO, "\n", 1);
    rl_done = 1;
    rl_point = rl_end = 0;
    rl_replace_line("", 0);
}

void normal_signal_handler(int signo)
{
    // debug_print_signal("normal_signal_handler", signo);
    
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

// Komut çalışırken sinyal handler'ı
void command_signal_handler(int signo)
{
    // debug_print_signal("command_signal_handler", signo);
    
    g_signal_number = signo;
    write(STDOUT_FILENO, "\n", 1);
    // Komut sinyal ile sonlandırıldı
}

// SIGQUIT handler'ı
void handle_sigquit(int signo)
{
    // debug_print_signal("handle_sigquit", signo);
    
    g_signal_number = signo;
    // SIGQUIT için özel işlem yapılmaz, sadece kaydedilir
}

// Normal sinyalleri kur (prompt'ta)
void setup_normal_signals(void)
{
    // fprintf(stderr, "[DEBUG] Setting up normal signals\n");
    // fflush(stderr);
    signal(SIGINT, normal_signal_handler);
    signal(SIGQUIT, handle_sigquit);
}

// Heredoc sinyallerini kur
void setup_heredoc_signals(void)
{
    // fprintf(stderr, "[DEBUG] Setting up heredoc signals\n");
    // fflush(stderr);
    signal(SIGINT, heredoc_signal_handler);
    signal(SIGQUIT, handle_sigquit);
}

// Komut sinyallerini kur
void setup_command_signals(void)
{
    // fprintf(stderr, "[DEBUG] Setting up command signals\n");
    // fflush(stderr);
    signal(SIGINT, command_signal_handler);
    signal(SIGQUIT, handle_sigquit);
}

// Sinyal durumunu sıfırla
void reset_signal_state(void)
{
    // fprintf(stderr, "[DEBUG] Resetting signal state\n");
    // fflush(stderr);
    g_signal_number = 0;
}

// Sinyal durumunu kontrol et ve gerekli exit code'u döndür
int check_signal_status(void)
{
    if (g_signal_number == SIGINT) {
        int exit_code = 130;
        g_signal_number = 0; // Sinyali sıfırla
        
        // Readline durumunu tamamen sıfırla - ÇOK ÖNEMLİ!
        rl_on_new_line();
        rl_replace_line("", 0);
        rl_redisplay();
        
        return exit_code;
    } else if (g_signal_number == SIGQUIT) {
        int exit_code = 131;
        g_signal_number = 0; // Sinyali sıfırla
        return exit_code;
    }
    return 0; // Sinyal yok
}