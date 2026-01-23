#include "expansion_utils.h"

// Determines if the char is a special variables
int is_special_char(char c)
{
    return c == '$' || c == '`' || c == '"' || c == '\\' || c == '\n';
}

/**
 * @brief 		Makes sure that the identifier is valid
 *
 * When the user tries to expand, he can pass some invalid variable names to
 * try to expand. but it should not be able to cuz the name is invalid.
 * Tho we need to check for its validty.
 *
 * @param name	The identifier to check
 *
 * @return		Success or Failure (1 or 0)
 */

int is_valid_identifier(char *name)
{
    if (name == NULL || name[0] == '\0')
        return 0;

    // The variable we try to expand can be a positional arg
    if (isdigit(name[0]))
    {
        for (int i = 0; name[i] != '\0'; i++)
        {
            if (isdigit(name[i]) == 0)
                return 0;
        }

        return 1;
    }

    // Else the name can only start with a letter or a _
    if (isalpha(name[0]) == 0 && name[0] != '_')
        return 0;

    for (int i = 1; name[i] != '\0'; i++)
    {
        if (isalnum(name[i]) == 0 && name[i] != '_')
            return 0;
    }

    return 1;
}
