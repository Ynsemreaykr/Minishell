#include "../include/minishell.h"
#include <stdlib.h>
#include <unistd.h>


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
        shell->env = ft_malloc(sizeof(char *) * 1, __FILE__, __LINE__);
        shell->env[0] = NULL;
        add_basic_env_vars(shell);
        return;
    }
    
    // Environment array'inin boyutunu hesapla
    int count = 0;
    while (envp[count])
        count++;
    
    // Array için yer aç (count + 1 for NULL terminator)
    shell->env = ft_malloc(sizeof(char *) * (count + 1), __FILE__, __LINE__);
    
    // Her string için ayrı yer aç ve PATH optimizasyonu yap
    int i = 0;
    while (i < count) {
        // PATH değişkenini olduğu gibi kopyala (optimize etme)
        if (!ft_strncmp(envp[i], "PATH=", 5)) {
            // PATH'i optimize etmeden olduğu gibi kopyala
            int len = ft_strlen(envp[i]);
            shell->env[i] = ft_malloc(len + 1, __FILE__, __LINE__);
            ft_strcpy(shell->env[i], envp[i]);
        } else {
            // Diğer environment variable'ları normal şekilde kopyala
            int len = ft_strlen(envp[i]);
            shell->env[i] = ft_malloc(len + 1, __FILE__, __LINE__);
            ft_strcpy(shell->env[i], envp[i]);
        }
        i++;
    }
    
    shell->env[count] = NULL; // NULL terminator
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
    ft_free(new_shlvl);
} 