NAME = minishell
SRCS = src/main.c src/parser.c src/pipe_parser.c src/tokenizer.c src/count_token_utils.c src/utils.c src/token_processing.c src/process_double_quote_utils.c src/builtin.c src/builtin_utils.c src/heredoc.c src/signals.c src/memory.c src/env.c src/executor.c src/command_handlers.c \
       LIBFT/ft_strlen.c LIBFT/ft_strcpy.c LIBFT/ft_strcat.c LIBFT/ft_strdup.c LIBFT/ft_strcmp.c LIBFT/ft_strncmp.c \
       LIBFT/ft_strchr.c LIBFT/ft_strncpy.c LIBFT/ft_atoi.c LIBFT/ft_calloc.c LIBFT/ft_memset.c LIBFT/ft_isalnum.c LIBFT/ft_isalpha.c \
       LIBFT/ft_putstr.c LIBFT/ft_strtok.c LIBFT/ft_expand_env_vars.c LIBFT/ft_expand_tilde.c LIBFT/ft_itoa.c
OBJS = \
	$(SRCS:.c=.o)
CC = cc
CFLAGS = -Wall -Wextra -Werror -fPIE
RM = rm -f

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS) -lreadline

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
