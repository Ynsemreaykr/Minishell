#include "../include/minishell.h"
#include <stdlib.h>
#include <stdio.h>

// Gelişmiş token parsing - quote ve redirection operatör işleme ile
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
            // Redirection değil, normal token olarak işle
            int start = i;
            int in_quote = 0;
            char quote_char = 0;
            
            // Bu bir quoted string mi kontrol et
            if (input[i] == '"' || input[i] == '\'') {
                quote_char = input[i];
                in_quote = 1;
                start = i;
                i++; // Açılış quote'unu atla
                
                // Kapanış quote'unu bul
                while (input[i] && input[i] != quote_char) {
                    i++;
                }
                if (input[i] == quote_char) {
                    i++; // Kapanış quote'unu atla
                }
                
                // Quoted string'i tek token olarak işle
                if (i > start) {
                    args[argc] = process_token_content(input, start, i, last_exit, shell);
                    if (args[argc]) {
                        argc++;
                    }
                }
            } else {
                // Normal token'ı işle (quoted content dahil)
                while (input[i] && ((input[i] != ' ' && input[i] != '\t') || in_quote)) {
                    if (!in_quote && check_redirection_operator(input, i)) {
                        break;
                    }
                    
                    // Quote'ları işle
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
                
                if (i > start) { // Token'ın başlangıcından sonuna kadar karakter varsa token say
                    args[argc] = process_token_content(input, start, i, last_exit, shell);
                    if (args[argc] && ft_strlen(args[argc]) > 0) {
                        argc++;
                    } else if (args[argc]) {
                        // Boş token'ı temizle
                        ft_free(args[argc]);
                    }
                }
            }
        } else {
            // Redirection operatörünü işle
            int redir_len = get_redirection_length(input, i);
            if (redir_len > 0) {
                char *redir_token = ft_malloc(redir_len + 1, __FILE__, __LINE__);
                ft_strncpy(redir_token, input + i, redir_len);
                redir_token[redir_len] = '\0';
                args[argc++] = redir_token;
                i += redir_len;
                
                // Eğer bu bir heredoc (<<) ise, sonraki token quote'ları korumalı
                if (redir_len == 2 && input[i-2] == '<' && input[i-1] == '<') {
                    // Boşlukları atla
                    i = skip_whitespace(input, i);
                    
                    // Heredoc delimiter'ı quote'lar korunarak işle
                    if (input[i]) {
                        int start = i;
                        int in_quote = 0;
                        char quote_char = 0;
                        
                        // Tüm delimiter'ı işle (quoted content dahil)
                        while (input[i] && ((input[i] != ' ' && input[i] != '\t') || in_quote)) {
                            // Sonraki redirection operatörünü kontrol et
                            if (!in_quote && i + 1 < (int)strlen(input) && 
                                input[i] == '<' && input[i+1] == '<') {
                                break; // Sonraki heredoc operatöründe dur
                            }
                            
                            // Quote'ları işle
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
                            // Delimiter'ı tam olarak kopyala (quote'lar korunarak)
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
        // Redirection operatörlerinde dur
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

// Input string'deki token sayısını say
int count_tokens(const char *input)
{
    int token_count = 0;
    int i = 0;
    int in_quote = 0;
    char quote_char = 0;
    
    while (input[i]) {
        i = skip_whitespace(input, i);
        if (!input[i]) break;
        
        // Quote'ları önce işle ve quote state'ini güncelle
        if ((input[i] == '"' || input[i] == '\'') && !in_quote) {
            quote_char = input[i];
            in_quote = 1;
            i++;
            // Quoted string'i token olarak say
            token_count++;
            // Quote kapanana kadar ilerle
            while (input[i] && input[i] != quote_char) {
                i++;
            }
            if (input[i] == quote_char) {
                i++; // Closing quote'u atla
            }
            in_quote = 0;
            quote_char = 0;
            continue;
        }
        
        // Redirection operatörlerini işle - ayrı token olarak say
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
