/*-------------------------------------------------------------------------
 *
 * getpeereid.c
 *		get peer userid for UNIX-domain socket connection
 *
 * Portions Copyright (c) 1996-2014, PostgreSQL Global Development Group
 *
 *
 * IDENTIFICATION
 *	  src/port/getpeereid.c
 *
 *-------------------------------------------------------------------------
 */

#include "c.h"

int pg_st_write(st_netfd_t sock, const void* udata, int length, int flags){
	void* data = st_netfd_getspecific(sock);
	int ret = st_write(sock,udata,length,data==NULL?-1:100);
	if( ret == -1 && data != NULL ){
		if( errno == ETIME ){
			errno = EAGAIN;
		}
	}
	return ret;
}
int pg_st_recv(st_netfd_t sock, void* udata, int length, int flags){
	void* data = st_netfd_getspecific(sock);
	int ret = st_read(sock,udata,length,data==NULL?-1:100);
	if( ret == -1 && data != NULL ){
		if( errno == ETIME ){
			errno = EAGAIN;
		}
	}
	return ret;
}
int pg_st_accept(st_netfd_t sock, void *arg1, void* len){
	void* data = st_netfd_getspecific(sock);
	int ret = st_accept(sock,arg1,len,data==NULL?-1:100);
	if( ret == -1 && data != NULL ){
		if( errno == ETIME ){
			errno = EAGAIN;
		}
	}
	return ret;
}


st_netfd_t pg_st_open_sock(int a, int b, int c){
	#undef socket
	int desc = socket(a, b, c);
	#define socket(a,b,c) pg_st_open_sock(a,b,c)
	return st_netfd_open_socket(desc);
}

void pg_st_close_sock(st_netfd_t sock){
	st_netfd_close(sock);
}
#include <sys/param.h>
#include <sys/socket.h>
#include <unistd.h>
#ifdef HAVE_SYS_UN_H
#include <sys/un.h>
#endif
#ifdef HAVE_UCRED_H
#include <ucred.h>
#endif
#ifdef HAVE_SYS_UCRED_H
#include <sys/ucred.h>
#endif


/*
 * BSD-style getpeereid() for platforms that lack it.
 */
int
getpeereid(int sock, uid_t *uid, gid_t *gid)
{
#if defined(SO_PEERCRED)
	/* Linux: use getsockopt(SO_PEERCRED) */
	struct ucred peercred;
	ACCEPT_TYPE_ARG3 so_len = sizeof(peercred);

	if (getsockopt(sock, SOL_SOCKET, SO_PEERCRED, &peercred, &so_len) != 0 ||
		so_len != sizeof(peercred))
		return -1;
	*uid = peercred.uid;
	*gid = peercred.gid;
	return 0;
#elif defined(LOCAL_PEERCRED)
	/* Debian with FreeBSD kernel: use getsockopt(LOCAL_PEERCRED) */
	struct xucred peercred;
	ACCEPT_TYPE_ARG3 so_len = sizeof(peercred);

	if (getsockopt(sock, 0, LOCAL_PEERCRED, &peercred, &so_len) != 0 ||
		so_len != sizeof(peercred) ||
		peercred.cr_version != XUCRED_VERSION)
		return -1;
	*uid = peercred.cr_uid;
	*gid = peercred.cr_gid;
	return 0;
#elif defined(HAVE_GETPEERUCRED)
	/* Solaris: use getpeerucred() */
	ucred_t    *ucred;

	ucred = NULL;				/* must be initialized to NULL */
	if (getpeerucred(sock, &ucred) == -1)
		return -1;

	*uid = ucred_geteuid(ucred);
	*gid = ucred_getegid(ucred);
	ucred_free(ucred);

	if (*uid == (uid_t) (-1) || *gid == (gid_t) (-1))
		return -1;
	return 0;
#else
	/* No implementation available on this platform */
	errno = ENOSYS;
	return -1;
#endif
}
