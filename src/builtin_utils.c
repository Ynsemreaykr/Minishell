#include "../include/minishell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const char	*cd_get_path(char **argv, int argc)
{
	const char	*path;

	if (argc > 2)
	{
		ft_putstr_fd("cd: too many arguments\n", 2);
		return (NULL);
	}
	if (argv[1])
		path = argv[1];
	else
		path = getenv("HOME");
	if (!path)
	{
		ft_putstr_fd("cd: HOME not set\n", 2);
		return (NULL);
	}
	if (argv[1] && (ft_strlen(argv[1]) == 0))
		return ("");
	return (path);
}

int	ft_cd(char **argv, t_shell *shell)
{
	int			argc;
	const char	*path;
	char		current_dir[1024];
	char		new_dir[1024];

	argc = 0;
	while (argv[argc])
		argc++;
	path = cd_get_path(argv, argc);
	if (!path)
		return (1);
	if (ft_strlen(path) == 0)
		return (0);
	if (getcwd(current_dir, sizeof(current_dir)) != NULL)
		set_env_var("OLDPWD", current_dir, shell);
	if (chdir(path) != 0)
	{
		perror("cd");
		return (1);
	}
	if (getcwd(new_dir, sizeof(new_dir)) != NULL)
		set_env_var("PWD", new_dir, shell);
	return (0);
}


int ft_unset(char **argv, t_shell *shell)
{
    int i;

    i = 1;
    while (argv[i])
    {
        unset_env_var(argv[i], shell);
        i++;
    }
    return 0;
}

int ft_env(char **argv, t_shell *shell)
{
    (void)argv;
    char **env;
    int i;

    env = get_env(shell);
    i = 0;
    while (env && env[i]) {
        if (strchr(env[i], '='))
            printf("%s\n", env[i]);
        i++;
    }
    return 0;
} 