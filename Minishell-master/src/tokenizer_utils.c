#include "../include/minishell.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Sayıyı string'e çevir
static void int_to_str(int num, char *str)
{
    int i = 0;
    int temp = num;
    
    // Sayının basamak sayısını hesapla
    int digits = 0;
    if (temp == 0) {
        digits = 1;
    } else {
        while (temp != 0) {
            temp /= 10;
            digits++;
        }
    }
    
    // Sayıyı string'e çevir
    str[digits] = '\0';
    for (i = digits - 1; i >= 0; i--) {
        str[i] = '0' + (num % 10);
        num /= 10;
    }
}

// Redirection operatörlerini kontrol et
int is_redirection_operator(const char *s, int i, int r_index)
{
    if ((s[i] == '>' || s[i] == '<')) {
        if (r_index == 0) 
            return 1; // Token başında ise 1
        else 
            return 2; // Token ortasında redirection operatörü
    }
    return 0; // Redirection yok
}

// $? için özel kontrol
int is_exit_status_var(const char *s, int i, int r_index)
{
    if (s[i] == '$' && s[i + 1] == '?') {
        if (r_index == 0) {
            return 1; // Token başında $?
        } else {
            return 2; // Token ortasında $?
        }
    }
    return 0; // $? değil
}

// Tırnak işaretlerini işle
void handle_quotes(const char *s, int *i, char *res, int *r_index, int *flags)
{
    char quote = s[*i]; // tırnak işaretini sakla
    res[(*r_index)++] = s[(*i)++]; // tırnak işaretini dahil et
    
    if (quote == '\'')
        *flags &= ~1; // expand flag'ini 0 yap (bit 0)
    
    // Tırnak var flag'ini set et (bit 2)
    *flags |= 4;
    
    while (s[*i] && s[*i] != quote)  // null olana kadar ve tırnak işareti gelen kadar döngüde kal
        res[(*r_index)++] = s[(*i)++];
    
    if (s[*i] == quote) { // tırnak kapatıldıysa, kapanmış tırnağı da dahil et
        res[(*r_index)++] = s[(*i)++];
    } else {
        *flags |= 2; // unclosed_quote flag'ini 1 yap (bit 1)
    }
}

// Tilde expansion yap
char *handle_tilde_expansion(char *res)
{
    if (res[0] == '~') 
    {
        char *expanded = ft_expand_tilde(res);
        if (expanded) {
            ft_free(res);
            return expanded;
        } else {
            // Eğer expansion başarısızsa, orijinal string'i kopyala
            char *copy = ft_strdup(res);
            ft_free(res);
            return copy;
        }
    }
    return res;
}

// Değişken adını çıkar
void extract_variable_name(const char *token, int var_start, char *varname)
{
    int var_end = var_start;
    
    if (token[var_start] == '?') {
        var_end = var_start + 1;
    } else if (token[var_start] == '_') {
        var_end = var_start + 1;
    } else if (token[var_start] == '"' || token[var_start] == '\'' || token[var_start] == '\0') {
        // $ sonrasında quote ya da string sonu geliyorsa, variable name yok
        var_end = var_start;
    } else {
        // Variable name sadece alphanumeric ve underscore karakterlerinden oluşabilir
        // Quote karakterlerinde (', ") dur
        while (token[var_end] && (ft_isalnum(token[var_end]) || token[var_end] == '_') &&
               token[var_end] != '\'' && token[var_end] != '"')
            var_end++;
    }
    
    ft_strncpy(varname, token + var_start, var_end - var_start);
    varname[var_end - var_start] = '\0';
}

// $? değişkenini genişlet
void expand_exit_status(char *buf, int *b, int last_exit)
{
    char numbuf[16];
    int_to_str(last_exit, numbuf);
    ft_strcat(buf, numbuf);
    *b += ft_strlen(numbuf);
}

// $_ değişkenini genişlet
void expand_last_arg(char *buf, int *b)
{
    void(*buf);
    void(*b);
    return;
}

// Environment değişkenini genişlet
void expand_env_variable(char *buf, int *b, const char *varname, char **envp)
{
    for (int e = 0; envp && envp[e]; e++) {
        if (!ft_strncmp(envp[e], varname, ft_strlen(varname)) && 
            envp[e][ft_strlen(varname)] == '=') {
            ft_strcat(buf, envp[e] + ft_strlen(varname) + 1);
            *b += ft_strlen(envp[e] + ft_strlen(varname) + 1);
            break;
        }
    }
}

// Args array'ini genişlet
char **resize_args_array(char **args, int *argc, int split_count)
{
    if (*argc + split_count > 0 && (*argc + split_count) % 32 == 0) {
        char **new_args = ft_malloc(sizeof(char *) * (*argc + split_count + 34), __FILE__, __LINE__);
        for (int k = 0; k < *argc; k++) 
            new_args[k] = args[k];
        ft_free(args);
        return new_args;
    }
    return args;
} 