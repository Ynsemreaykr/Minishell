#include "../include/minishell.h"
#include <stdlib.h>
#include <stdio.h>

// Get the length of redirection operator at current position
int get_redirection_length(const char *input, int pos)
{
    if (input[pos] == '<') {
        if (input[pos + 1] == '<') 
            return 2; // <<
        return 1; // <
    }
    if (input[pos] == '>') {
        if (input[pos + 1] == '>') 
            return 2; // >>
        return 1; // >
    }
    return 0;
}

// Check if current position is a redirection operator (returns 1 if true, 0 if false)
int check_redirection_operator(const char *input, int pos)
{
    if (input[pos] == '<' || input[pos] == '>') {
        // Check for double operators (<< or >>)
        if ((input[pos] == '<' && input[pos + 1] == '<') || 
            (input[pos] == '>' && input[pos + 1] == '>')) {
            return 1;
        }
        // Single operator (< or >)
        return 1;
    }
    return 0;
}

// Skip whitespace and return new position
int skip_whitespace(const char *input, int pos)
{
    while (input[pos] == ' ' || input[pos] == '\t') 
        pos++;
    return pos;
}

// Update quote state in token counting
void update_quote_state(const char *input, int *pos, int *in_quote, char *quote_char)
{
    if ((input[*pos] == '"' || input[*pos] == '\'') && !*in_quote) {
        *quote_char = input[*pos];
        *in_quote = 1;
        (*pos)++;
    } else if (input[*pos] == *quote_char && *in_quote) {
        *in_quote = 0;
        *quote_char = 0;
        (*pos)++;
    } else {
        (*pos)++;
    }
}