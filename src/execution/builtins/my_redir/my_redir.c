#include "my_redir.h"

int do_redir(struct AST *root, struct AST **redir);

//###############
//#   HELPERS   #
//###############

/**
 * @brief			Mimics the redirections > and >|
 *
 * Executes a redirection between two file descriptors.
 * Replaces the content inside the file by the new content (stdout of a command)
 *
 * @param command	The command to execute (the one to be redirected)
 * @param redir		The full list of remaining redirection to execute
 * @param fd		The file descriptor in which we want to redirect the content
 *
 * @return			Succes or Failure (0 or 1)
 */

static int redir_replace_in(struct AST *root, struct AST **redir, int fd)
{
    int fd_file = open(redir[0]->children[1]->content,
                       O_CREAT | O_WRONLY | O_TRUNC, 0644);

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
 * @brief			Mimics the redirections >>
 *
 * Executes a redirection between two file descriptors.
 * Appends the content after the content that was already present in the file
 *
 * @param command	The command to execute (the one to be redirected)
 * @param redir		The full list of remaining redirection to execute
 * @param fd		The file descriptor in which we want to redirect the content
 *
 * @return			Succes or Failure (0 or 1)
 */

static int redir_append_in(struct AST *root, struct AST **redir, int fd)
{
    int fd_file = open(redir[0]->children[1]->content,
                       O_CREAT | O_WRONLY | O_APPEND, 0644);

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
 * @brief			Mimics the redirections <
 *
 * Executes a redirection between two file descriptors.
 * Reads the content inside the file to use it as the stdin of the command
 *
 * @param command	The command to execute (the one to be redirected)
 * @param redir		The full list of remaining redirection to execute
 * @param fd		The file descriptor in which we want to redirect the content
 *
 * @return			Succes or Failure (0 or 1)
 */

static int redir_read(struct AST *root, struct AST **redir, int fd)
{
    int fd_file = open(redir[0]->children[1]->content, O_RDONLY);

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

/**
 * @brief			Mimics the redirections >& and <&
 *
 * Executes a redirection between two file descriptors.
 * Open the file descriptor for reading and writting inside depending on the
 * precised file descriptor
 *
 * @param command	The command to execute (the one to be redirected)
 * @param redir		The full list of remaining redirection to execute
 * @param fd		The file descriptor in which we want to redirect the content
 *
 * @return			Succes or Failure (0 or 1)
 */

static int redir_open(struct AST *root, struct AST **redir, int fd)
{
    int fd_file = open(redir[0]->children[1]->content, O_RDWR | O_CREAT, 0644);

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

	// IF FOR EACH REDIR
	if (strcmp(content, ">") == 0 || strcmp(content, ">|") == 0)
		status = redir_replace_in(root, redir, fd == -1 ? 1 : fd);
	else if (strcmp(content, ">>") == 0)
		status = redir_append_in(root, redir, fd == -1 ? 1 : fd);
	else if (strcmp(content, "<") == 0)
		status = redir_read(root, redir, fd == -1 ? 0 : fd);
	else if (strcmp(content, ">&") == 0)
		status = redir_dup(root, redir, fd == -1 ? 1 : fd);
	else if (strcmp(content, "<&") == 0)
		status = redir_dup(root, redir, fd == -1 ? 0 : fd);
	else if (strcmp(content, "<>") == 0)
		status = redir_open(root, redir, fd == -1 ? 0 : fd);

	free(to_free);

	if (status != 0)
		return status;

	return status;
}
