#include "../include/minishell.h"
#include <stdlib.h>
#include <unistd.h>

// g_env artık g_shell.env olarak kullanılıyor

// Environment string'i oluştur
static char *create_env_string(const char *name, const char *value)
{
    if (value) {
        // Değeri olan değişken (boş string de olsa)
        int name_len = ft_strlen(name);
        int value_len = ft_strlen(value);
        char *env_str = malloc(name_len + value_len + 2);
        ft_strcpy(env_str, name);
        ft_strcat(env_str, "=");
        ft_strcat(env_str, value);
        return env_str;
    } else {
        // Sadece ismi olan değişken (export NAME gibi) - = karakteri yok
        char *env_str = malloc(ft_strlen(name) + 1);
        ft_strcpy(env_str, name);
        return env_str;
    }
}

// Yeni environment variable ekle
static int add_new_env_var(const char *name, const char *value, t_shell *shell)
{
    // Mevcut environment'ın boyutunu hesapla
    int count = 0;
    while (shell->env[count])
        count++;
    
    // Yeni environment array'i oluştur
    char **new_env = malloc(sizeof(char *) * (count + 2));
    if (!new_env)
        return -1;
    
    // Mevcut değişkenleri kopyala
    int i = 0;
    while (i < count) {
        new_env[i] = shell->env[i];
        i++;
    }
    
    // Yeni değişkeni ekle
    new_env[count] = create_env_string(name, value);
    new_env[count + 1] = NULL;
    
    // Eski environment'ı temizle ve yenisini ata
    free(shell->env);
    shell->env = new_env;
    
    return 0;
}

// Temel environment değişkenlerini ekle
static void add_basic_env_vars(t_shell *shell)
{
    // PWD - Current working directory
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd))) {
        set_env_var("PWD", cwd, shell);
    }
    
    // SHLVL - Shell level (env -i ile başlatıldığında 0)
    set_env_var("SHLVL", "0", shell);
    
    // _ - Last command (env -i ile başlatıldığında /usr/bin/env)
    set_env_var("_", "/usr/bin/env", shell);
}

// Environment variable'ları başlat
void init_env(char **envp, t_shell *shell)
{
    if (!envp || !envp[0]) {
        // env -i ile başlatıldığında temel değişkenleri ekle
        shell->env = malloc(sizeof(char *) * 1);
        shell->env[0] = NULL;
        add_basic_env_vars(shell);
        return;
    }
    
    // Environment array'inin boyutunu hesapla
    int count = 0;
    while (envp[count])
        count++;
    
    // Array için yer aç (count + 1 for NULL terminator)
    shell->env = malloc(sizeof(char *) * (count + 1));
    
    // Her string için ayrı yer aç
    int i = 0;
    while (i < count) {
        int len = ft_strlen(envp[i]);
        shell->env[i] = malloc(len + 1);
        ft_strcpy(shell->env[i], envp[i]);
        i++;
    }
    
    shell->env[count] = NULL; // NULL terminator
}

// Environment variable'ları temizle
void cleanup_env(t_shell *shell)
{
    if (shell->env) {
        int i = 0;
        while (shell->env[i]) {
            free(shell->env[i]);
            i++;
        }
        free(shell->env);
        shell->env = NULL;
    }
}

// Environment variable'ları al
char **get_env(t_shell *shell)
{
    return shell->env;
}

// Environment variable ara
char *get_env_var(const char *name, t_shell *shell)
{
    if (!shell->env || !name)
        return NULL;
    
    int name_len = ft_strlen(name);
    int i = 0;
    while (shell->env[i]) {
        if (ft_strncmp(shell->env[i], name, name_len) == 0 && shell->env[i][name_len] == '=')
            return shell->env[i] + name_len + 1;
        i++;
    }
    return NULL;
}

// Environment variable ekle/güncelle
int set_env_var(const char *name, const char *value, t_shell *shell)
{
    if (!name)
        return -1;
    
    // Önce mevcut değişkeni ara
    int name_len = ft_strlen(name);
    int existing_index = -1;
    
    int i = 0;
    while (shell->env[i]) {
        if (ft_strncmp(shell->env[i], name, name_len) == 0 && 
            (shell->env[i][name_len] == '=' || shell->env[i][name_len] == '\0')) {
            existing_index = i;
            break;
        }
        i++;
    }
    
    // Mevcut değişkeni güncelle
    if (existing_index != -1) {
        free(shell->env[existing_index]);
        shell->env[existing_index] = create_env_string(name, value);
        return 0;
    }
    
    // Yeni değişken ekle
    return add_new_env_var(name, value, shell);
}

// Environment variable sil
int unset_env_var(const char *name, t_shell *shell)
{
    if (!shell->env || !name)
        return -1;
    
    int name_len = ft_strlen(name);
    int i = 0;
    while (shell->env[i]) {
        // Hem = karakteri olan hem de olmayan değişkenleri kontrol et
        if (ft_strncmp(shell->env[i], name, name_len) == 0 && 
            (shell->env[i][name_len] == '=' || shell->env[i][name_len] == '\0')) {
            // Değişkeni sil
            free(shell->env[i]);
            // Sonraki değişkenleri öne kaydır
            int j = i;
            while (shell->env[j]) {
                shell->env[j] = shell->env[j + 1];
                j++;
            }
            return 0;
        }
        i++;
    }
    return -1;
} 

// SHLVL değişkenini güncelle
void update_shlvl(t_shell *shell)
{
    char *shlvl_str = get_env_var("SHLVL", shell);
    int shlvl_value = 0; // Varsayılan değer
    
    if (shlvl_str) {
        shlvl_value = ft_atoi(shlvl_str);
        if (shlvl_value < 0)
            shlvl_value = 0;
    }
    
    // SHLVL değerini bir arttır
    shlvl_value++;
    
    // Yeni değeri string'e çevir
    char *new_shlvl = ft_itoa(shlvl_value);
    
    // SHLVL değişkenini güncelle
    set_env_var("SHLVL", new_shlvl, shell);
    
    // Geçici string'i temizle
    free(new_shlvl);
} 