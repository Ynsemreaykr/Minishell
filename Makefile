NAME = minishell
LIBFT_DIR = LIBFT
LIBFT = $(LIBFT_DIR)/libft.a

SRCS = src/main.c src/process_expansion_outside_quotes.c src/create_redir_process.c src/calculate_required.c src/signal_utils.c src/apply_redir_builtin.c src/parse_command.c src/sort_env.c src/apply_redir_child.c src/expand_env_content.c src/heredoc_child.c src/create_redir.c src/cmd_loop.c src/parse_heredoc.c src/heredoc_quotes.c src/pipe_check.c src/main_input_check.c src/token_heredoc.c src/ft_unset.c src/main_utils.c src/ft_export_two.c src/read_content.c src/pipe_parser.c src/input_check_utils.c src/execute_command.c src/tokenizer.c src/ft_export.c src/exec_builtin.c src/expand_heredoc_size.c src/tokenizer_utils.c src/env_utils.c src/heredoc_expand.c src/count_token_check.c src/count_token_utils.c src/child_process.c src/setup_built_redir.c src/exec_pipeline.c src/cleanup_utils.c src/cleanup.c src/token_processing.c src/process_double_quote_utils.c src/builtin.c src/builtin_utils.c src/heredoc.c src/signals.c src/memory.c src/env.c src/executor.c src/command_handlers.c src/command_executors.c src/parse_path.c

LIBFT_SRCS = LIBFT/ft_strlen.c LIBFT/ft_strcpy.c LIBFT/ft_strcat.c LIBFT/ft_strdup.c LIBFT/ft_strcmp.c LIBFT/ft_strncmp.c \
             LIBFT/ft_strchr.c LIBFT/ft_strncpy.c LIBFT/ft_atoi.c LIBFT/ft_memset.c LIBFT/ft_isalnum.c LIBFT/ft_isalpha.c \
             LIBFT/ft_putstr_fd.c LIBFT/ft_itoa.c LIBFT/ft_split.c LIBFT/ft_strjoin.c

OBJS = $(SRCS:.c=.o)
LIBFT_OBJS = $(LIBFT_SRCS:.c=.o)

CC = cc
CFLAGS = -Wall -Wextra -Werror
RM = rm -f

all: $(NAME)

$(LIBFT): $(LIBFT_OBJS)
	ar rcs $(LIBFT) $(LIBFT_OBJS)

$(NAME): $(OBJS) $(LIBFT)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(LIBFT) -lreadline

clean:
	$(RM) $(OBJS) $(LIBFT_OBJS)

fclean: clean
	$(RM) $(NAME) $(LIBFT)

re: fclean all

.PHONY: all clean fclean re
 