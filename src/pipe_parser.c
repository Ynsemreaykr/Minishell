#include "../include/minishell.h"
#include <string.h>
#include <stdlib.h>

// Quote state'i toggle et (aç/kapat)
static void toggle_quote_state(char c, int *in_single_quote, int *in_double_quote)
{
    if (c == '\'' && !*in_double_quote) 
        *in_single_quote = !*in_single_quote;
    else if (c == '"' && !*in_single_quote) 
        *in_double_quote = !*in_double_quote;
}

// Input string'deki pipe sayısını hesapla
static int count_pipes(const char *input)
{
    int pipe_count = 0;
    int i = 0;
    int in_single_quote = 0;
    int in_double_quote = 0;
    
    while (input[i]) 
    {
        toggle_quote_state(input[i], &in_single_quote, &in_double_quote);
        
        if (input[i] == '|' && !in_single_quote && !in_double_quote) 
            pipe_count++;
        i++;
    }
    
    return pipe_count;
}

// Komut string'ini commands array'ine ekle
static void add_command_to_array(char **commands, int *cmd_count, const char *input, int start, int end)
{
    if (end > start) 
    {
        int len = end - start;
        commands[*cmd_count] = ft_malloc(len + 1, __FILE__, __LINE__);
        ft_strncpy(commands[*cmd_count], input + start, len);
        commands[*cmd_count][len] = '\0';
        (*cmd_count)++;
    }
}

// Ana parsing loop'unu çalıştır
static void parse_pipe_loop(char **commands, int *cmd_count, const char *input, int *start, int *i)
{
    int in_single_quote = 0;
    int in_double_quote = 0;
    
    while (input[*i]) 
    {
        toggle_quote_state(input[*i], &in_single_quote, &in_double_quote);
        
        if (input[*i] == '|' && !in_single_quote && !in_double_quote) 
        {
            add_command_to_array(commands, cmd_count, input, *start, *i);
            *start = (*i) + 1;
        }
        (*i)++;
    }
}

// Quote'ları dikkate alarak pipe splitting yap
char **split_by_pipes(const char *input, int *count)
{
    int pipe_count = count_pipes(input);
    int max_commands = pipe_count + 1;
    char **commands = ft_malloc(sizeof(char *) * (max_commands + 1), __FILE__, __LINE__);
    int cmd_count = 0;
    int start = 0;
    int i = 0;
    
    parse_pipe_loop(commands, &cmd_count, input, &start, &i);
    add_command_to_array(commands, &cmd_count, input, start, i);
    
    commands[cmd_count] = NULL;
    *count = cmd_count;
    return commands;
}
