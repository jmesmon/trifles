/*
 * Based on ccan/grab_file, which is:
 *
 * Licensed under LGPLv2+ - see LICENSE file for details
 */
#pragma once
#include <stdio.h> // For size_t

/**
 * grab_fd - read all of a file descriptor into memory
 * @fd: the file descriptor to read from
 * @size: the (optional) size of the file
 *
 * This function reads from the given file descriptor until no more
 * input is available.  The content is malloced(), and the size
 * of the file places in @size if it's non-NULL.  For convenience, the
 * byte after the end of the content will always be NUL.
 *
 * Example:
 *	// Return the first line.
 *	static char *read_stdin_firstline(void)
 *	{
 *		char *all, *nl;
 *
 *		all = grab_fd(0, NULL);
 *		if (!all)
 *			return NULL;
 *		nl = strchr(all, '\n');
 *		if (nl)
 *			*nl = '\0';
 *		return all;
 *	}
 */
void *grab_fd(int fd, size_t *size);

/**
 * grab_file - read all of a file (or stdin) into memory
 * @filename: the file to read (NULL for stdin)
 * @size: the (optional) size of the file
 *
 * This function reads from the given file until no more input is
 * available.  The content is malloed(), and the size of the
 * file places in @size if it's non-NULL.  For convenience, the byte
 * after the end of the content will always be NUL.
 *
 * Example:
 *	static char *read_file_firstline(const char *filename)
 *	{
 *		char *nl, *all;
 *
 *		all = grab_file(filename, NULL);
 *		if (!all)
 *			return NULL;
 *		nl = strchr(all, '\n');
 *		if (nl)
 *			*nl = '\0';
 *		return all;
 *	}
 */
void *grab_file(const char *filename, size_t *size);
