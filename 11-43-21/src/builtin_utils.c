#define _GNU_SOURCE
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

static int is_valid_identifier(const char *name)
{
    int i = 0;
    
    // İlk karakter letter veya _ olmalı
    if (!name[0] || (!ft_isalpha(name[0]) && name[0] != '_'))
        return 0;
    
    // Geri kalan karakterler alphanumeric veya _ olmalı
    i = 1;
    while (name[i])
    {
        if (!ft_isalnum(name[i]) && name[i] != '_')
            return 0;
        i++;
    }
    return 1;
}



int ft_export(char **argv, t_shell *shell)
{
    int i = 1;
    
    // Eğer argüman yoksa, tüm export edilmiş değişkenleri listele
    if (!argv[i]) {
        char **env = get_env(shell);
        int count = 0;
        
        // Önce değişken sayısını say
        while (env && env[count])
            count++;
        
        // Değişkenleri geçici dizide sakla ve sırala
        char **sorted_env = ft_malloc(sizeof(char *) * (count + 1), __FILE__, __LINE__);
        for (int j = 0; j < count; j++) {
            sorted_env[j] = ft_strdup(env[j]);
        }
        sorted_env[count] = NULL;
        
        // Basit bubble sort ile sırala
        for (int j = 0; j < count - 1; j++) {
            for (int k = 0; k < count - j - 1; k++) {
                if (ft_strcmp(sorted_env[k], sorted_env[k + 1]) > 0) {
                    char *temp = sorted_env[k];
                    sorted_env[k] = sorted_env[k + 1];
                    sorted_env[k + 1] = temp;
                }
            }
        }
        
        // Sıralanmış değişkenleri yazdır
        for (int j = 0; j < count; j++) {
            char *eq = strchr(sorted_env[j], '=');
            if (eq) {
                *eq = '\0';
                // _ değişkenini export listesinde gösterme
                if (ft_strcmp(sorted_env[j], "_") != 0) {
                    printf("declare -x %s=\"%s\"\n", sorted_env[j], eq + 1);
                }
                *eq = '=';
            } else {
                // Değeri olmayan değişkenler için sadece isim göster
                // _ değişkenini export listesinde gösterme
                if (ft_strcmp(sorted_env[j], "_") != 0) {
                    printf("declare -x %s\n", sorted_env[j]);
                }
            }
            ft_free(sorted_env[j]);
        }
        ft_free(sorted_env);
        return 0;
    }
    
    while (argv[i])
    {
        char *eq = strchr(argv[i], '=');
        
        // Değişken ismini kontrol et
        char *var_name;
        if (eq) {
            int name_len = eq - argv[i];
            var_name = ft_malloc(name_len + 1, __FILE__, __LINE__);
            ft_strncpy(var_name, argv[i], name_len);
            var_name[name_len] = '\0';
        } else {
            var_name = ft_strdup(argv[i]);
        }
        
        // Geçersiz identifier kontrolü
        if (!is_valid_identifier(var_name)) {
            ft_putstr_fd("minishell: export: `", 2);
            ft_putstr_fd(argv[i], 2);
            ft_putstr_fd("': not a valid identifier\n", 2);
            ft_free(var_name);
            return 1;
        }
        
        // Eğer token a= ile bitiyorsa ve sonraki token varsa, birleştir
        if (eq && eq[1] == '\0' && argv[i + 1]) {
            // Tüm sonraki token'ları birleştir
            int total_len = ft_strlen(argv[i]);
            int j = i + 1;
            while (argv[j]) {
                total_len += ft_strlen(argv[j]) + 1; // +1 for space
                j++;
            }
            
            char *combined = ft_malloc(total_len + 1, __FILE__, __LINE__);
            ft_strcpy(combined, argv[i]);
            
            j = i + 1;
            while (argv[j]) {
                ft_strcat(combined, " ");
                ft_strcat(combined, argv[j]);
                j++;
            }
            
            char *new_eq = strchr(combined, '=');
            if (new_eq) {
                *new_eq = 0;
                // Başındaki boşluğu temizle
                char *value = new_eq + 1;
                while (*value == ' ') value++;
                set_env_var(combined, value, shell);
                *new_eq = '=';
            }
            ft_free(combined);
            ft_free(var_name);
            // Tüm sonraki token'ları atla
            while (argv[i]) i++;
            continue;
        }
        
        if (eq)
        {
            *eq = 0;
            set_env_var(var_name, eq + 1, shell);
            *eq = '=';
        }
        else
        {
            // Sadece isim verilmişse (export a gibi), değişkeni export et
            // Eğer değişken zaten varsa, hiçbir şey yapma (değeri koru)
            // Eğer değişken yoksa, sadece isim olarak ekle
            char *existing_value = get_env_var(var_name, shell);
            if (!existing_value) {
                // Değişken yok, sadece isim olarak ekle
                set_env_var(var_name, NULL, shell);
            }
            // Eğer değişken zaten varsa, hiçbir şey yapma (değeri koru)
        }
        ft_free(var_name);
        i++;
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