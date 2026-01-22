#include "my_redir.h"

int do_redir(struct AST *root, struct AST **redir);

//###############
//#   HELPERS   #
//###############

/**
 * @brief			Mimics all the directions working with a file
 *
 * Executes a redirection between two file descriptors.
 *
 * @param command	The command to execute (the one to be redirected)
 * @param redir		The full list of remaining redirection to execute
 * @param fd		The file descriptor in which we want to redirect the content
 * @param flags		The flags with which we open the file depending on the
 * 					redir
 *
 * @return			Succes or Failure (0 or 1)
 */

static int general_redir(struct AST *root, struct AST **redir,
		int fd, int flags)
{
    int fd_file = open(redir[0]->children[1]->content, flags, 0644);

    int fd_save = dup(fd);

    if (dup2(fd_file, fd) == -1)
    {
        fprintf(stderr, "Error: Could not dup\n");
        return 1;
    }
    close(fd_file);

    int status = do_redir(root, redir + 1);

    if (dup2(fd_save, fd) == -1)
    {
        fprintf(stderr, "Error: Could not dup\n");
        return 1;
    }
    close(fd_save);

    return status;
}

/**
 * @brief			Mimics the redirections >& and <&
 *
 * Executes a redirection between two file descriptors.
 * Dupplicates the file descriptor passed as parameters in the precised file
 * descriptor
 * If the file descriptor is '-' then we close the precised file descriptor
 *
 * @param command	The command to execute (the one to be redirected)
 * @param redir		The full list of remaining redirection to execute
 * @param fd		The file descriptor in which we want to redirect the content
 *
 * @return			Succes or Failure (0 or 1)
 */


static int redir_dup(struct AST *root, struct AST **redir, int fd)
{
    char *word = redir[0]->children[1]->content;

    if (strlen(word) != 1
        || (word[0] != '-' && (word[0] < '0' || '9' < word[0])))
    {
        fprintf(stderr, "Error: >&: Wrong FD word = '%s'\n", word);
        return 1;
    }

    int fd_save = dup(fd);

    if (word[0] == '-')
        close(fd);

    else
    {
        int fd_file = word[0] - '0';

        if (fd != fd_file)
        {
            if (dup2(fd_file, fd) == -1)
            {
                fprintf(stderr, "Error: Could not dup\n");
                return 1;
            }
            // close(fd_file);
        }
    }

    int status = do_redir(root, redir + 1);

    if (dup2(fd_save, fd) == -1)
    {
        fprintf(stderr, "Error: Could not dup\n");
        return 1;
    }
    close(fd_save);

    return status;
}

//#################
//#   MAIN FUNC   #
//#################

/**
 * @brief		Executes a redir
 *
 * Various REDIR exists so for each type we excute a different one.
 * In the program we first execute all the REDIR before executing the command
 *
 * @param root	The AST node to execute
 * @param redir The rest of REDIT to execute
 *
 * @return		Exit code
 */

struct redir redirs_table[] =
{
	{">", O_CREAT | O_WRONLY | O_TRUNC, 1},
	{">|", O_CREAT | O_WRONLY | O_TRUNC, 1},
	{">>", O_CREAT | O_WRONLY | O_APPEND, 1},
	{"<", O_RDONLY, 0},
	{"<>", O_RDWR | O_CREAT, 0},
	{"<&", 0, 0},
	{">&", 0, 1},
	{NULL, 0, -1}
};

int execute_redir(struct AST *root, struct AST **redir)
{
	int fd = -1;

	// Get the IONumber of the REDIR
	char ionumber = redir[0]->children[0]->content[0];
	char *content = strdup(redir[0]->children[0]->content);
	char *to_free = content;

	// Check if a IONumber is given if not we keep the default one for each
	// redir
	if (ionumber >= '0' && ionumber <= '9')
	{
		fd = ionumber - '0';
		content++;
	}

	int status = 0;

	int flags = 0; 
	
	for (int i = 0; redirs_table[i].type != NULL; i++)
	{
		if (strcmp(content, redirs_table[i].type) == 0)
		{
			if (fd == -1)
				fd = redirs_table[i].fd;

			flags = redirs_table[i].flags;
		}
	}

	if (strcmp(content, ">&") == 0 || strcmp(content, "<&") == 0 )
		status = redir_dup(root, redir, fd);
	else
		status = general_redir(root, redir, fd, flags);

	free(to_free);

	if (status != 0)
		return status;

	return status;
}
