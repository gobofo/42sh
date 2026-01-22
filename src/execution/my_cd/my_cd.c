#include "my_cd.h"

extern struct env *env;

/**
 * @brief			Creates the absolute path from a path
 *
 * Creates the absolute path of a given path by concatenation with the PWD
 *
 * @param cur_path	The current relativ path
 *
 * @return			The new absolute path
 */

static char *create_path(char *cur_path)
{
    // Check if the current path is an absolute or no

    // If the path starts with a / is an absolute path and we do nothing
    if (cur_path[0] == '/')
        return strdup(cur_path);

    // If it is not an absolute path we must do a concatenation:
    // '/'PWD'/'path

    char *pwd = hash_map_get(env->variables, "PWD");
	char *save_pwd = NULL;

	if (pwd == NULL)
	{
		save_pwd = getcwd(NULL, 0);
		pwd = save_pwd;
	}

	if (pwd == NULL || pwd[0] == '\0')
	{
		if (save_pwd)
			free(save_pwd);

		return NULL;
	}

    // Compute the absolute path length
    // PWD + '/' + path
	size_t pwd_len = strlen(pwd);
    size_t abs_path_len = pwd_len + 1 + strlen(cur_path);

    // Create the absolute path by concatenation
    char *abs_path = malloc(abs_path_len + 1);
    if (abs_path == NULL)
        return NULL;

    strcpy(abs_path, pwd);

    // If the PWD does not end with a / we add it
    if (pwd_len > 0 && abs_path[strlen(pwd) - 1] != '/')
        strcat(abs_path, "/");

    strcat(abs_path, cur_path);

	if (save_pwd)
		free(save_pwd);

    return abs_path;
}

/**
 * @brief			Reconstructs a path from a stack
 *
 * @param stack		The stack with all the components
 * @param top		The index of the top element of the stack, representing
 * 					the size
 *
 * @return			The path
 */

static char *reconstruct_path(char **stack, size_t top)
{
    char *path;
    size_t len = 0;

    FILE *stream = open_memstream(&path, &len);

    fputc('/', stream);

    for (int i = 0; stack[i] != NULL && top > 0; i++)
    {
        fputs(stack[i], stream);

        if (stack[i + 1] != NULL)
            fputc('/', stream);

        top--;
    }

    fclose(stream);

    return path;
}

/**
 * @brief			Creates the canonical form of a path
 *
 * This process consists to clean up the path.
 * If a '.' is found then it is deleted from the path so is the / following it.
 * If a '..' component is found then the last component is removed if possible
 * else an error message is send.
 *
 * @param cur_path	The current path to convert
 *
 * @return			The canonical form of the path
 */

static char *canonical_form(char *cur_path)
{
    char *path_copy = strdup(cur_path);

    // We use a stack to hold all components to handle the ..
    // The path will be created once the path has been processed entirely from
    // the stack.
    size_t top = 0;

    char **stack = calloc(strlen(cur_path), sizeof(char *));

    char *token = strtok(path_copy, "/");
    while (token != NULL)
    {
        // If the component is '..' we pop the last element found
        if (strcmp(token, "..") == 0)
        {
            // Ensure we have something to pop
            if (top > 0)
                top--;
        }
        // Since we ignore the '.' we dont have to take them in consideration
        else if (strcmp(token, ".") != 0)
        {
            stack[top++] = token;
        }

        token = strtok(NULL, "/");
    }

    char *path = reconstruct_path(stack, top);
    if (path == NULL)
        return NULL;

    free(stack);
    free(path_copy);

    return path;
}

/**
 * @brief			Mimics the builtin cd command
 *
 * Goes to a specified directory in the computer
 *
 * @param command	The array of command representing the potential flags and
 * 					the path to go to
 *
 * @return			Succes or Failure (0 or 1)
 */

int my_cd(char **command)
{
    // TODO
    // Handle if no args are given -> Step 4 with implementation of the HOME
    // env var

    // Since the subject does not require to handle the -L and -P flags, cd
    // must have exactly one single argument
    // To remove when HOME var is added
    if (command == NULL || command[0] == NULL)
    {
        fprintf(stderr, "Error: cd: not enough arguments\n");
        return 1;
    }

    if (command[1] != NULL)
    {
        fprintf(stderr, "Error: cd: too many arguments\n");
        return 2;
    }

	if (strcmp(command[0], "-") == 0)
	{
        char *pwd = hash_map_get(env->variables, "PWD");
        char *old = hash_map_get(env->variables, "OLDPWD");
		
		if (old == NULL)
		{
			fprintf(stderr, "Error: cd : OLDPWD not set\n");
			return 1;
		}

		char *target = strdup(old);

		// Swap pwd
		hash_map_insert(env->variables, "OLDPWD", strdup(pwd), free); 
		hash_map_insert(env->variables, "PWD", strdup(old), free);

		// Check if paths exists
		if (chdir(target) != 0)
		{
			fprintf(stderr, "Error: cd: no such file or directory: %s\n",
					command[0]);

			return 1;
		}

		printf("%s\n", target);
        free(target);

		return 0;
    }

    char *cur_path = create_path(command[0]);
    if (cur_path == NULL)
        return 1;

    // Get the canonical form of the path
    char *path = canonical_form(cur_path);
    if (path == NULL)
    {
        free(cur_path);
        return 1;
    }

    free(cur_path);

    // Check if paths exists
    if (chdir(path) != 0)
    {
        fprintf(stderr, "Error: cd: no such file or directory: %s\n",
                command[0]);

        free(path);

        return 1;
    }

    char *pwd = hash_map_get(env->variables, "PWD");


	// Update the PWD and OLDPWD
	hash_map_insert(env->variables, "OLDPWD", strdup(pwd), free); 
	hash_map_insert(env->variables, "PWD", strdup(path), free);

    free(path);

    return 0;
}
