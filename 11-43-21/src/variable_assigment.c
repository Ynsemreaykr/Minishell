#include "../include/minishell.h"


// Check if a command is a variable assignment (VAR=value format)
int is_variable_assignment(char **argv)
{
    if (!argv || !argv[0])
        return 0;
    
    // Check if the first argument contains '=' and is not a command
    char *equals = ft_strchr(argv[0], '=');
    if (!equals || equals == argv[0])
        return 0; // No '=' or starts with '='
    
    // Check if everything before '=' is a valid variable name
    char *p = argv[0];
    while (p < equals) {
        if (!ft_isalnum(*p) && *p != '_')
            return 0;
        p++;
    }
    
    return 1; // Valid variable assignment
}   


// Execute variable assignment
int execute_variable_assignment(char **argv, t_shell *shell)
{
    if (!argv || !argv[0])
        return 1;
        
    char *equals = ft_strchr(argv[0], '=');
    if (!equals)
        return 1;
    
    // Split into variable name and value
    int name_len = equals - argv[0];
    char *var_name = ft_malloc(name_len + 1, __FILE__, __LINE__);
    ft_strncpy(var_name, argv[0], name_len);
    var_name[name_len] = '\0';
    
    char *var_value = equals + 1;
    
    // Set the environment variable using the shell's env management
    set_env_var(var_name, var_value, shell);
    
    ft_free(var_name);
    return 0;
}
