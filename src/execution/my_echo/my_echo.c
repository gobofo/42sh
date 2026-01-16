#include "my_echo.h"
#include <string.h>
/**
 * @brief		Checks if the flags passed to the echo function are valid
 *
 * The user can pass some flags to the echo function to alter the way the
 * function works.
 * Tho some flags are not valid, so we want to avoid the user to use non
 * existing flags.
 *
 * @param flags	The flags entered by the user
 *
 * @return		Succes or Failure (1 or 0)
 */

int is_valid_echo_flags(char *flags)
{
    if (flags[0] != '-')
        return 0;

    int i = 1;

    while (flags[i])
    {
        if (flags[i] != 'n' && flags[i] != 'e' && flags[i] != 'E')
            return 0;

        i++;
    }

    return 1;
}

/**
 * @brief			Set the right flags for the echo function
 *
 * Some flags can be associated with other while other are overridden by other
 * flags.
 * So we need to keep track of this by checking the flags one by one.
 *
 * @param command	The full command the user wants to use
 * @param flag_e	To know if the flag e is active (1) or inactive (0)
 * @param flag_n	To know if the flag n is active (1) or inactive (0)
 *
 * @return			The current flags to use
 */
int set_flags_echo(char **command, int *flag_e, int *flag_n)
{
    int i = 0;

    while (command[i] != NULL && is_valid_echo_flags(command[i]))
    {
        int j = 1;

        while (command[i][j])
        {
            if (command[i][j] == 'e')
                *flag_e = 1;

            if (command[i][j] == 'E')
                *flag_e = 0;

            if (command[i][j] == 'n')
                *flag_n = 1;

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
                str[j + 1] = str[i + 1];
                j++;
                i++;
            }
        }
        else
            str[j] = str[i];

        i++;
        j++;
    }

    str[j] = '\0';
}

/**
 * @brief			Mimic the echo shell function
 *
 * @param command	The command to execute
 *
 * @return			Succes or Failure (0 or 1) of the function
 */

int my_echo(char **command)
{
    int flag_n = 0;
    int flag_e = 0;

    int i_debut = set_flags_echo(command, &flag_e, &flag_n);

    command += i_debut;

    while (*command != NULL)
    {
        if (flag_e)
            expand_backslash(*command);

        printf("%s", *command);

        if (*(command + 1) != NULL && strcmp(*(command+1),"")!=0)
            printf(" ");

        command++;
    }

    if (!flag_n)
        printf("\n");

    fflush(stdout);
    return 0;
}
