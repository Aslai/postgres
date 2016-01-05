/*-------------------------------------------------------------------------
 *
 * noblock.c
 *	  set a file descriptor as non-blocking
 *
 * Portions Copyright (c) 1996-2014, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * IDENTIFICATION
 *	  src/port/noblock.c
 *
 *-------------------------------------------------------------------------
 */

#include "c.h"

#include <fcntl.h>


bool
pg_set_noblock(pgsocket sock)
{
	st_netfd_setspecific(sock, (void*) 1, NULL);
	return true;
#if !defined(WIN32)
	//return (fcntl(st_netfd_fileno(sock), F_SETFL, O_NONBLOCK) != -1);
#else
	//unsigned long ioctlsocket_ret = 1;

	/* Returns non-0 on failure, while fcntl() returns -1 on failure */
	//return (ioctlsocket(st_netfd_fileno(sock), FIONBIO, &ioctlsocket_ret) == 0);
#endif
}


bool
pg_set_block(pgsocket sock)
{
	st_netfd_setspecific(sock, (void*) 0, NULL);
	return true;
#if !defined(WIN32)
	//int			flags;

	//flags = fcntl(st_netfd_fileno(sock), F_GETFL);
	//if (flags < 0 || fcntl(st_netfd_fileno(sock), F_SETFL, (long) (flags & ~O_NONBLOCK)))
	//	return false;
	//return true;
#else
	//unsigned long ioctlsocket_ret = 0;

	/* Returns non-0 on failure, while fcntl() returns -1 on failure */
	//return (ioctlsocket(st_netfd_fileno(sock), FIONBIO, &ioctlsocket_ret) == 0);
#endif
}
