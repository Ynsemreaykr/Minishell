#define _GNU_SOURCE
#include "../include/minishell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int is_n_flag(const char *arg)
{
    if (!arg || arg[0] != '-')
        return 0;
    
    // - ile başlayan ve sadece n karakterlerinden oluşan argümanları -n flag'i olarak kabul et
    for (int i = 1; arg[i]; i++)
    {
        if (arg[i] != 'n')
            return 0;
    }
    return 1;
}

static void print_echo_args(char **argv, int start)
{
    for (int i = start; argv[i]; i++)
    {
        printf("%s", argv[i]);
        if (argv[i + 1])
            printf(" ");
    }
}

int ft_echo(char **argv)
{
    int n_flag = 0;
    int i = 1;
    
    // Sadece baştaki -n/-nnn... flaglerini işle, ilk farklı argümandan sonra kalanları normal yazdır
    while (argv[i] && is_n_flag(argv[i]))
    {
        n_flag = 1;
        i++;
    }
    print_echo_args(argv, i);
    if (!n_flag)
        printf("\n");
    return 0;
}

static int is_valid_number(const char *str)
{
    int i = 0;
    
    // Skip leading/trailing quotes - they should be removed by parser
    // but check just in case
    const char *start = str;
    const char *end = str + ft_strlen(str) - 1;
    
    // İlk karakter + veya - olabilir
    if (start[i] == '+' || start[i] == '-')
        i++;
    
    // En az bir digit olmalı
    if (!start[i])
        return 0;
    
    // Geri kalan karakterler digit olmalı
    while (start[i] && start + i <= end)
    {
        if (start[i] < '0' || start[i] > '9')
            return 0;
        i++;
    }
    return 1;
}

int ft_exit(char **argv)
{
    int arg_count = 0;
    
    // Argüman sayısını say
    while (argv[arg_count])
        arg_count++;
    
    // Hiç argüman yoksa 0 ile çık
    if (arg_count == 1) {
        exit(0);
    }
    
    // İkinci argüman geçerli bir sayı mı kontrol et
    if (!is_valid_number(argv[1])) {
        ft_putstr_fd("minishell: exit: ", 2);
        ft_putstr_fd(argv[1], 2);
        ft_putstr_fd(": numeric argument required\n", 2);
        exit(2);
    }
    
    // Fazla argüman varsa hata ver ama çıkma
    if (arg_count > 2) {
        ft_putstr_fd("minishell: exit: too many arguments\n", 2);
        return 1; // Shell'i kapatma, sadece hata döndür
    }
    
    // Geçerli sayı ile çık
    int code = ft_atoi(argv[1]);
    exit(code);
}

int ft_pwd(char **argv)
{
    char cwd[1024];
    (void)argv;
    if (getcwd(cwd, sizeof(cwd)))
        printf("%s\n", cwd);
    else
        perror("pwd");
    return 0;
}

int is_builtin(const char *cmd)
{
    if (!cmd)
        return 0;
        
    return (
        ft_strcmp(cmd, "echo") == 0 ||
        ft_strcmp(cmd, "exit") == 0 ||
        ft_strcmp(cmd, "pwd") == 0 ||
        ft_strcmp(cmd, "env") == 0 ||
        ft_strcmp(cmd, "cd") == 0 ||
        ft_strcmp(cmd, "export") == 0 ||
        ft_strcmp(cmd, "unset") == 0
    );
} 