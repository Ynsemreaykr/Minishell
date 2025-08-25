#include "../include/minishell.h"
#include <fcntl.h>

// < file
int handle_redir_in(t_redir *redir)
{
    if (access(redir->filename, F_OK) != 0) {
        ft_putstr_fd(redir->filename, 2);
        ft_putstr_fd(": No such file or directory\n", 2);
        ft_mem_cleanup();
        _exit(1);
    }
    int in = open(redir->filename, O_RDONLY);
    if (in < 0) {
        ft_mem_cleanup();
        _exit(1);
    }
    dup2(in, 0);
    close(in);
    return 0;
}

// > file
int handle_redir_out(t_redir *redir)
{
    int out = open(redir->filename, O_WRONLY|O_CREAT|O_TRUNC, 0644);
    if (out < 0) {
        perror(redir->filename);
        ft_mem_cleanup();
        _exit(1);
    }
    dup2(out, 1);
    close(out);
    return 0;
}

// >> file
int handle_redir_append(t_redir *redir)
{
    int out = open(redir->filename, O_WRONLY|O_CREAT|O_APPEND, 0644);
    if (out < 0) {
        perror(redir->filename);
        ft_mem_cleanup();
        _exit(1);
    }
    dup2(out, 1);
    close(out);
    return 0;
}

// Son heredoc içeriğini bir pipe’a yazıp stdin’e bağlar.
// input_redirected set edilir (1). İçerik yoksa dokunmaz.
void setup_heredoc_stdin(t_cmd *cmd, int *input_redirected)
{
    if (!cmd->heredocs) return;

    // Son heredoc’u bul
    t_heredoc *h = cmd->heredocs;
    while (h->next) 
        h = h->next;

    if (h->content && !*input_redirected) {
        int hpipe[2];
        if (pipe(hpipe) == 0) {
            // İçeriği yaz → yazma ucunu kapat → stdin’i okuma ucuna bağla
            write(hpipe[1], h->content, ft_strlen(h->content));
            close(hpipe[1]);
            dup2(hpipe[0], STDIN_FILENO);
            close(hpipe[0]);
            *input_redirected = 1;
        }
    }
}

// Redir listesini sırayla uygular. Başarıyla uygularsa uygun flag’leri set eder.
void setup_redir_list(t_cmd *cmd, int *input_redirected, int *output_redirected)
{
    if (!cmd->redirs) return;

    for (t_redir *r = cmd->redirs; r; r = r->next) {
        if (r->type == REDIR_IN) {
            handle_redir_in(r);
            *input_redirected = 1;
        } else if (r->type == REDIR_OUT) {
            handle_redir_out(r);
            *output_redirected = 1;
        } else if (r->type == REDIR_APPEND) {
            handle_redir_append(r);
            *output_redirected = 1;
        }
        // İleride 2>, &>, 1>&2 gibi tipler eklenecekse buraya case ekleyebilirsin.
    }
}



