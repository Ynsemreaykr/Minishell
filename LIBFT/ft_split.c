#include "../include/minishell.h"

// String'i belirtilen delimiter ile ayırıp array döndürür
static int count_words(const char *s, char c)
{
    int count = 0;
    int in_word = 0;
    
    while (*s) {
        if (*s != c && !in_word) {
            in_word = 1;
            count++;
        } else if (*s == c) {
            in_word = 0;
        }
        s++;
    }
    return count;
}

static char *get_next_word(const char **s, char c)
{
    const char *start;
    int len = 0;
    
    // Delimiter'ları atla
    while (**s == c)
        (*s)++;
    
    start = *s;
    
    // Kelime uzunluğunu hesapla
    while (**s && **s != c) {
        len++;
        (*s)++;
    }
    
    if (len == 0)
        return NULL;
    
    // Kelimeyi kopyala
    char *word = ft_malloc(len + 1, __FILE__, __LINE__);
    ft_strncpy(word, start, len);
    word[len] = '\0';
    
    return word;
}

char **ft_split(const char *s, char c)
{
    if (!s)
        return NULL;
    
    int word_count = count_words(s, c);
    char **result = ft_malloc(sizeof(char *) * (word_count + 1), __FILE__, __LINE__);
    
    int i = 0;
    const char *ptr = s;
    
    while (i < word_count) {
        char *word = get_next_word(&ptr, c);
        if (word)
            result[i++] = word;
        else
            break;
    }
    
    result[i] = NULL;
    return result;
}

// ft_split ile oluşturulan array'i temizle
void ft_split_free(char **split_result)
{
    if (!split_result)
        return;
    
    int i = 0;
    while (split_result[i]) {
        ft_free(split_result[i]);
        i++;
    }
    ft_free(split_result);
}
