#include "../include/minishell.h"
#include <string.h>
#include <stdlib.h>



// Process escape sequences in double quotes
void process_escape_sequence(const char *input, int *i, char *processed, int *proc_len)
{
    (*i)++; // Skip backslash
    processed[*proc_len] = input[*i];
    (*proc_len)++;
    (*i)++;
}

// Process exit status expansion ($?)
void process_exit_status(int last_exit, char *processed, int *proc_len)
{
    char *exit_str = ft_itoa(last_exit);
    if (exit_str) {
        ft_strcpy(processed + *proc_len, exit_str);
        *proc_len += ft_strlen(exit_str);
        ft_free(exit_str);
    }
}

// Process variable expansion in double quotes
void process_variable_expansion_in_quotes(const char *input, int *i, int end, char *processed, int *proc_len, t_shell *shell)
{
    (*i)++; // Skip $
    int name_start = *i;
    while (*i < end && input[*i] != '"' && (ft_isalnum(input[*i]) || input[*i] == '_')) {
        (*i)++;
    }
    
    // Extract variable name - dinamik bellek
    int name_length = *i - name_start;
    if (name_length > 0) {
        char *env_var_name = ft_malloc(name_length + 1, __FILE__, __LINE__);
        ft_strncpy(env_var_name, input + name_start, name_length);
        env_var_name[name_length] = '\0';
        
        // Get environment variable value
        char *env_value = get_env_var(env_var_name, shell);
        
        // Add variable value or empty if not found
        if (env_value) {
            ft_strcpy(processed + *proc_len, env_value);
            *proc_len += ft_strlen(env_value);
        }
        
        ft_free(env_var_name);
    }
}

// Process variable expansion outside quotes
void process_variable_expansion_outside_quotes(const char *input, int *i, int end, char *processed, int *proc_len, t_shell *shell)
{
    (*i)++; // Skip $
    int name_start = *i;
    while (*i < end && (ft_isalnum(input[*i]) || input[*i] == '_') && 
           input[*i] != ' ' && input[*i] != '\t' && input[*i] != '"' && input[*i] != '\'') {
        (*i)++;
    }
    
    // Extract variable name - dinamik bellek
    int name_length = *i - name_start;
    if (name_length > 0) {
        char *env_var_name = ft_malloc(name_length + 1, __FILE__, __LINE__);
        ft_strncpy(env_var_name, input + name_start, name_length);
        env_var_name[name_length] = '\0';
        
        // Get environment variable value
        char *env_value = get_env_var(env_var_name, shell);
        
        // Add variable value or empty if not found
        if (env_value) {
            ft_strcpy(processed + *proc_len, env_value);
            *proc_len += ft_strlen(env_value);
        }
        
        ft_free(env_var_name);
    }
}

// Process literal $ character
void process_literal_dollar(char *processed, int *proc_len, int *i)
{
    processed[*proc_len] = '$';
    (*proc_len)++;
    (*i)++;
}

// Process regular character
void process_regular_char(const char *input, int *i, char *processed, int *proc_len)
{
    processed[*proc_len] = input[*i];
    (*proc_len)++;
    (*i)++;
}
