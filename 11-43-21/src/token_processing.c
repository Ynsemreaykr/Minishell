#include "../include/minishell.h"
#include <string.h>
#include <stdlib.h>

// Process double quote content with variable expansion
static char *process_double_quote(const char *input, int *i, int end, int last_exit, char *processed, int *proc_len, t_shell *shell)
{
    (*i)++; // Skip opening quote
    while (*i < end && input[*i] != '"') 
    {
        if (input[*i] == '\\' && *i + 1 < end && (input[*i + 1] == '"' || input[*i + 1] == '\\' || input[*i + 1] == '$')) 
        {  // kaçış karakteri ile " \ veya $ kullanıldıysa kaçış karakterini sayma
            process_escape_sequence(input, i, processed, proc_len);
        } 
        else if (input[*i] == '$') 
        {
            // Variable expansion in double quotes
            if (*i + 1 < end && input[*i + 1] == '"') 
            { // "$" durumu
                process_literal_dollar(processed, proc_len, i);
            } 
            else if (*i + 1 < end && input[*i + 1] == '?') 
            {
                // $? special case
                process_exit_status(last_exit, processed, proc_len);
                *i += 2; // Skip $?
            } 
            else if (*i + 1 < end && (ft_isalnum(input[*i + 1]) || input[*i + 1] == '_')) 
            {
                // Veri expand edilmesi -> "$HOME" gibi
                process_variable_expansion_in_quotes(input, i, end, processed, proc_len, shell);
            } 
            else 
            { // expand edilememiş $ -> $yunus taki $ işareti atanır
                process_literal_dollar(processed, proc_len, i);
            }
        } 
        else 
        {
            process_regular_char(input, i, processed, proc_len);
        }
    }
    if (*i < end && input[*i] == '"') 
    {
        (*i)++; // Skip closing quote
    }
    return processed;
}

// Process single quote content (no variable expansion)
static char *process_single_quote(const char *input, int *i, int end, char *processed, int *proc_len)
{
    (*i)++; // Skip opening quote
    while (*i < end && input[*i] != '\'') {
        processed[*proc_len] = input[*i];
        (*proc_len)++;
        (*i)++;
    }
    if (*i < end && input[*i] == '\'') {
        (*i)++; // Skip closing quote
    }
    return processed;
}

// Process variable expansion outside quotes
static char *process_variable_outside_quotes(const char *input, int *i, int end, int last_exit, char *processed, int *proc_len, t_shell *shell)
{
    if (*i + 1 < end && input[*i + 1] == '?') {
        // $? special case - ft_itoa kullanarak
        char *exit_str = ft_itoa(last_exit);
        if (exit_str) {
            ft_strcpy(processed + *proc_len, exit_str);
            *proc_len += ft_strlen(exit_str);
            ft_free(exit_str);
        }
        *i += 2; // Skip $?
    } else if (*i + 1 < end && (ft_isalnum(input[*i + 1]) || input[*i + 1] == '_')) {
        // Variable expansion outside quotes
        process_variable_expansion_outside_quotes(input, i, end, processed, proc_len, shell);
    } else {
        // Just $ or invalid
        process_literal_dollar(processed, proc_len, i);
    }
    return processed;
}

// Helper function to extract and process a token with proper quote and variable handling
char *process_token_content(const char *input, int start, int end, int last_exit, t_shell *shell)
{
    // Dinamik bellek kullanımı - input uzunluğuna göre
    int max_len = (end - start) * 4; // Variable expansion için 4 kat daha fazla
    char *processed = ft_malloc(max_len, __FILE__, __LINE__);
    int proc_len = 0;
    int i = start;
    
    while (i < end) {
        if (input[i] == '"') {
            process_double_quote(input, &i, end, last_exit, processed, &proc_len, shell);
        } else if (input[i] == '\'') {
            process_single_quote(input, &i, end, processed, &proc_len);
        } else if (input[i] == '$') {
            process_variable_outside_quotes(input, &i, end, last_exit, processed, &proc_len, shell);
        } else {
            // Regular character
            processed[proc_len] = input[i];
            proc_len++;
            i++;
        }
    }
    
    processed[proc_len] = '\0';
    return processed;
}
