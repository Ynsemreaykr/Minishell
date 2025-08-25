#include "../include/minishell.h"


// Environment string'i oluştur
static char *create_env_string(const char *name, const char *value)
{
    if (value) {
        // Değeri olan değişken (boş string de olsa)
        int name_len = ft_strlen(name);
        int value_len = ft_strlen(value);
        char *env_str = ft_malloc(name_len + value_len + 2, __FILE__, __LINE__);
        if (!env_str)
            return NULL;
        ft_strcpy(env_str, name);
        ft_strcat(env_str, "=");
        ft_strcat(env_str, value);
        return env_str;
    } 
    else {
        // Sadece ismi olan değişken (export NAME gibi) - = karakteri yok
        char *env_str = ft_malloc(ft_strlen(name) + 1, __FILE__, __LINE__);
        if (!env_str)
            return NULL;
        ft_strcpy(env_str, name);
        return env_str;
    }
}



static int add_new_env_var(const char *name, const char *value, t_shell *shell)
{
    int count = 0;
    while (shell->env[count])
        count++;
    
    char **new_env = ft_malloc(sizeof(char *) * (count + 2), __FILE__, __LINE__);
    if (!new_env)
        return -1;
    
    int i = 0;
    while (i < count) {
        new_env[i] = shell->env[i];
        i++;
    }
    
    new_env[count] = create_env_string(name, value);
    new_env[count + 1] = NULL;
    
    ft_free(shell->env);
    shell->env = new_env;
    
    return 0;
}


int set_env_var(const char *name, const char *value, t_shell *shell)
{
    if (!name)
        return -1;
    
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
    
    if (existing_index != -1) {
        ft_free(shell->env[existing_index]);
        shell->env[existing_index] = create_env_string(name, value);
        return 0;
    }
    
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
            ft_free(shell->env[i]);
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

