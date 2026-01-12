#include "execution.h"

#include "../expansion/expansion.h"

int execute_node(struct AST *root);

/**
 * @brief			Mimic the echo shell function
 *
 * @param command	The command to execute
 *
 * @return			Succes or Failure (0 or 1) of the function
 */

int is_valid_echo_flags(char *flags)
{
	if(flags[0]!='-') return 0;

	int i=1;

	while(flags[i]){
		if(flags[i]!='n' && flags[i]!='e' && flags[i]!='E'){
			return 0;
		}
		i++;
	}
	return 1;
}

int set_flags_echo(char** command,int* flag_e, int* flag_n){

	int i=0;
	while(command[i]!=NULL && is_valid_echo_flags(command[i])){
		int j=1;
		while(command[i][j]){
			if(command[i][j]=='e'){
				*flag_e=1;
			}
			if(command[i][j]=='E'){
				*flag_e=0;
			}
			if (command[i][j]=='n'){
				*flag_n=1;
			}
			j++;
		}
		i++;
	}
	return i;
}

void expand_backslash(char *str)
{
    int i = 0;
    int j = 0;

    while (str[i])
    {
        if (str[i] == '\\' && str[i + 1])
        {
            if (str[i + 1] == 'n')
            {
                str[j] = '\n';
                i++;
            }
            else if (str[i + 1] == 't')
            {
                str[j] = '\t';
                i++;
            }
            else if (str[i + 1] == '\\')
            {
                str[j] = '\\';
                i++;
            }
            else
            {
                str[j] = str[i];
				str[j+1] = str[i+1];
				j++;
				i++;
            }
        }
        else
        {
            str[j] = str[i];
        }
        i++;
        j++;
    }
    str[j] = '\0';
}


int my_echo(char **command)
{

	int flag_n = 0;
	int flag_e = 0;

	int i_debut = set_flags_echo(command, &flag_e, &flag_n);

	command+=i_debut;

	while (*command != NULL)
	{
		if(flag_e)
			expand_backslash(*command);
		printf("%s", *command);
		
		if (*(command + 1) != NULL)
			printf(" ");

		command++;
	}

	if(!flag_n)
		printf("\n");

	fflush(stdout);
	return 0;
}


/**
 * @brief 		Creates the command with children from a node
 *
 * For a given node of the AST tree of type AST_CMD, creates a string from the
 * command node
 *
 * @param root	The parent root with all children beeing the commands
 *
 * @return		A string for command
 */
char **create_command(struct AST *root)
{
    // Is free by caller
    char **command = calloc(root->count_children + 1, sizeof(char *));

    for (int i = 0; i < root->count_children; i++)
    {
        expand(&root->children[i]->content);
        command[i] = root->children[i]->content;
    }

    return command;
}

int execute_cmd(struct AST *root)
{
    char **command = create_command(root);

    int status = 0;

    // We consider the quote expansion has been done
    if (strcmp(command[0], "true") == 0)
        status = 0;
    else if (strcmp(command[0], "false") == 0)
        status = 1;
    else if (strcmp(command[0], "echo") == 0)
        status = my_echo(command + 1);
    else
		// TODO - Use execvp
        fprintf(stderr, "Error: undefined command: \"%s\"\n", command[0]);
    free(command);

    return status;
}

int execute_if(struct AST *root)
{
    int condition = execute_node(root->children[0]);

    int status = 0;

    if (condition == 0)
        status = execute_node(root->children[1]);
    else if (root->count_children > 2)
        status = execute_node(root->children[2]);

    return status;
}

int execute_list(struct AST *root)
{
    int status = 0;

    for (int i = 0; i < root->count_children; i++)
        status = execute_node(root->children[i]);

    return status;
}

int execute_node(struct AST *root)
{
    switch (root->rule)
    {
    case AST_LIST:
        return execute_list(root);

    case AST_CMD:
        return execute_cmd(root);

    case AST_IF:
        return execute_if(root);

    // Not supposed to get there but we never know
    default:
        return 0;
    }
}

int execute_ast(struct AST *root)
{
    if (!root)
        return 1;

    return execute_node(root);
}
