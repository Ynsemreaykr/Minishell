#include "../include/minishell.h"
#include <stdlib.h>
#include <stdio.h>

// Enhanced token parsing with proper quote and redirection operator handling
char **split_tokens(const char *input, int last_exit, t_shell *shell)
{
    int token_count = count_tokens(input);
    
    if (token_count == -1) {
        return NULL;
    }
    
    char **args = ft_malloc(sizeof(char *) * (token_count + 1), __FILE__, __LINE__);
    int argc = 0;
    int i = 0;
    
    while (input[i]) {
        i = skip_whitespace(input, i);
        if (!input[i]) 
            break;
        
        // Check for redirection operators first
        if (!check_redirection_operator(input, i)) {
            // Not a redirection, process as regular token
            int start = i;
            int in_quote = 0;
            char quote_char = 0;
            
            // Process the entire token (including quoted content)
            while (input[i] && ((input[i] != ' ' && input[i] != '\t') || in_quote)) {
                if (!in_quote && check_redirection_operator(input, i)) {
                    break;
                }
                
                // Handle quotes
                if ((input[i] == '"' || input[i] == '\'') && !in_quote) {
                    quote_char = input[i];
                    in_quote = 1;
                    i++;
                } else if (input[i] == quote_char && in_quote) {
                    in_quote = 0;
                    quote_char = 0;
                    i++;
                } else {
                    i++;
                }
            }
            
            if (i > start) {
                args[argc] = process_token_content(input, start, i, last_exit, shell);
                if (args[argc]) {
                    argc++;
                }
            }
        } else {
            // Process redirection operator
            int redir_len = get_redirection_length(input, i);
            if (redir_len > 0) {
                char *redir_token = ft_malloc(redir_len + 1, __FILE__, __LINE__);
                ft_strncpy(redir_token, input + i, redir_len);
                redir_token[redir_len] = '\0';
                args[argc++] = redir_token;
                i += redir_len;
                
                // If this is a heredoc (<<), the next token should preserve quotes
                if (redir_len == 2 && input[i-2] == '<' && input[i-1] == '<') {
                    // Skip whitespace
                    i = skip_whitespace(input, i);
                    
                    // Process heredoc delimiter with quotes preserved
                    if (input[i]) {
                        int start = i;
                        int in_quote = 0;
                        char quote_char = 0;
                        
                        // Process the entire delimiter (including quoted content)
                        while (input[i] && ((input[i] != ' ' && input[i] != '\t') || in_quote)) {
                            // Handle quotes
                            if ((input[i] == '"' || input[i] == '\'') && !in_quote) {
                                quote_char = input[i];
                                in_quote = 1;
                                i++;
                            } else if (input[i] == quote_char && in_quote) {
                                in_quote = 0;
                                quote_char = 0;
                                i++;
                            } else {
                                i++;
                            }
                        }
                        
                        if (i > start) {
                            // Copy the delimiter exactly as is (with quotes preserved)
                            int delim_len = i - start;
                            char *delim_token = ft_malloc(delim_len + 1, __FILE__, __LINE__);
                            ft_strncpy(delim_token, input + start, delim_len);
                            delim_token[delim_len] = '\0';
                            args[argc++] = delim_token;
                        }
                    }
                }
            }
        }
    }
    
    args[argc] = NULL;
    return args;
}

static int process_regular_token(const char *input, int *pos, int *in_quote, char *quote_char)
{
    int token_count = 0;
    int start_pos = *pos;
    
    while (input[*pos] && ((input[*pos] != ' ' && input[*pos] != '\t') || *in_quote)) {
        // Stop at redirection operators
        if (!*in_quote && check_redirection_operator(input, *pos)) {
            break;
        }
        update_quote_state(input, pos, in_quote, quote_char);
    }
    
    // Eğer token başlangıcından sonuna kadar karakter varsa token say
    if (*pos > start_pos) {
        token_count = 1;
    }
    
    return token_count;
}

// Count tokens in input string
int count_tokens(const char *input)
{
    int token_count = 0;
    int i = 0;
    int in_quote = 0;
    char quote_char = 0;
    
    while (input[i]) {
        i = skip_whitespace(input, i);
        if (!input[i]) break;
        

        
        // Process quotes first to update quote state
        if ((input[i] == '"' || input[i] == '\'') && !in_quote) {
            quote_char = input[i];
            in_quote = 1;
            i++;
            continue;
        } 
        else if (input[i] == quote_char && in_quote) {
            in_quote = 0;
            quote_char = 0;
            i++;
            continue;
        }
        
        // Handle redirection operators - count as separate tokens
        if (!in_quote && check_redirection_operator(input, i)) {
            int redir_len = get_redirection_length(input, i);
            if (redir_len > 0) {
                token_count++;
                i += redir_len;
                continue;
            }
        }
        
        int regular_token_count = process_regular_token(input, &i, &in_quote, &quote_char);
        if (regular_token_count > 0) {
            token_count++;
        }
    }
    
    return token_count;
}
