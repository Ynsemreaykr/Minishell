#include "../include/minishell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int ft_cd(char **argv, t_shell *shell)
{
    // Argüman sayısını kontrol et
    int argc = 0;
    while (argv[argc])
        argc++;
    
    // cd 2'den fazla argüman almamalı (cd ve path)
    if (argc > 2) {
        ft_putstr_fd("cd: too many arguments\n", 2);
        return 1;
    }
    
    const char *path = argv[1] ? argv[1] : getenv("HOME");
    if (!path)
    {
        ft_putstr_fd("cd: HOME not set\n", 2);
        return 1;
    }
    
    // Boş string kontrolü - boş string'ler mevcut dizinde kalır
    if (argv[1] && (ft_strlen(argv[1]) == 0))
        return 0;
    
    // Mevcut dizini al ve OLDPWD olarak kaydet
    char current_dir[1024];
    if (getcwd(current_dir, sizeof(current_dir)) != NULL)
    {
        set_env_var("OLDPWD", current_dir, shell);
    }
    
    if (chdir(path) != 0)
    {
        perror("cd");
        return 1;
    }
    
    // Yeni dizini al ve PWD olarak kaydet
    char new_dir[1024];
    if (getcwd(new_dir, sizeof(new_dir)) != NULL)
    {
        set_env_var("PWD", new_dir, shell);
    }
    
    return 0;
}

int ft_unset(char **argv, t_shell *shell)
{
    int i = 1;
    while (argv[i])
    {
        unset_env_var(argv[i], shell);
        i++;
    }
    return 0;
}

int ft_env(char **argv, t_shell *shell)
{
    (void)argv;
    char **env = get_env(shell);
    for (int i = 0; env && env[i]; i++) {
        // Sadece değeri olan değişkenleri göster (= karakteri olan)
        if (strchr(env[i], '='))
            printf("%s\n", env[i]);
    }
    return 0;
} 