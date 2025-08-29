#include "../include/minishell.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>




char	**parse_path(char **envp)
{
	int		i;
	char	*env_path;

	i = 0;
	env_path = NULL;
	while (envp[i])
	{
		if (!ft_strncmp(envp[i], "PATH=", 5))
		{
			env_path = envp[i] + 5;
			break ;
		}
		i++;
	}
	if (!env_path)
		return (NULL);
	return (ft_split(env_path, ':'));
}

static int	search_in_path(char *command,
		char **splitted_path, char **full_path)
{
	int		i;
	char	*temp_path;

	if (!splitted_path)
		return (-1);
	i = 0;
	while (splitted_path[i])
	{
		temp_path = ft_strjoin(splitted_path[i], "/");
		*full_path = ft_strjoin(temp_path, command);
		ft_free(temp_path);
		if (access(*full_path, X_OK) == 0)
			return (0);
		ft_free(*full_path);
		i++;
	}
	*full_path = NULL;
	return (-1);
}


int	is_accessable(char *command, char **splitted_path, char **full_path)
{
	struct stat	path_stat;

	if (ft_strchr(command, '/'))
	{
		if (stat(command, &path_stat) == 0 && S_ISDIR(path_stat.st_mode))
			return (-3);
		if (access(command, F_OK) != 0)
			return (-4);
		if (access(command, X_OK) != 0)
			return (-2);
		*full_path = ft_strdup(command);
		return (0);
	}
	return (search_in_path(command, splitted_path, full_path));
}   



// 3. Execve işlemi - Komutu çalıştır
void execute_command(t_cmd *cmd, char **envp)
{
    char **splitted_path = parse_path(envp);
    char *full_path = NULL;
    int result;
    
    result = is_accessable(cmd->argv[0], splitted_path, &full_path);
    
    if (splitted_path)
        ft_split_free(splitted_path);
    if (result != 0) {
        if (result == -3) {
            ft_putstr_fd(cmd->argv[0], 2);
            ft_putstr_fd(": Is a directory\n", 2);
            ft_mem_cleanup();
            exit(126);
        } else if (result == -4) {
            ft_putstr_fd(cmd->argv[0], 2);
            ft_putstr_fd("minishell : No such file or directory\n", 2);
            ft_mem_cleanup();
            exit(127);
        } else if (result == -2) {
            ft_putstr_fd(cmd->argv[0], 2);
            ft_putstr_fd(": Permission denied\n", 2);
            ft_mem_cleanup();
            exit(126);
        } else {
            ft_putstr_fd(cmd->argv[0], 2);
            ft_putstr_fd(": command not found\n", 2);
            ft_mem_cleanup();
            exit(127);
        }
    }
    
    // Komutu çalıştır
    execve(full_path, cmd->argv, envp);
    perror("execve");
    ft_free(full_path); 
}


// KULLANILMIYOR
char	*find_path(char *cmd, char **envp)
{
	char	**splitted_path;
	char	*full_path;
	int		result;

	full_path = NULL;
	if (!cmd || ft_strlen(cmd) == 0)
		return (NULL);
	splitted_path = parse_path(envp);
	result = is_accessable(cmd, splitted_path, &full_path);
	if (splitted_path)
		ft_split_free(splitted_path);
	if (result == 0)
		return (full_path);
	return (NULL);
}

