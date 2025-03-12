/*
 * Copyright (C) 2014 - David Goulet <dgoulet@ev0ke.net>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License, version 2 only, as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>

#include <lib/torsocks.h>

#include <tap/tap.h>

#define NUM_TESTS 8

/* Suppress output messages. */
int tsocks_loglevel = MSGNONE;
//int tsocks_loglevel = MSGDEBUG;

static void test_connect_deny(void)
{
	int fd, ret;
	struct sockaddr_in sin = {0};
	struct sockaddr_in6 sin6 = {0};

	fd = tsocks_libc_socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
	skip_start(fd == -1, 1, "Couldn't create raw socket");
	sin.sin_family = AF_INET;
	ret = connect(fd, (struct sockaddr *) &sin, sizeof(sin));
	ok (ret == -1 && errno == EPERM,
	    "Connect with RAW socket NOT valid. ret=%d, errno=%d: %s",
	    ret, errno, strerror(errno));
	close(fd);
	skip_end();

	sin.sin_family = AF_INET;
	fd = tsocks_libc_socket(sin.sin_family, SOCK_DGRAM, 0);
	ret = connect(fd, (struct sockaddr *) &sin, sizeof(sin));
	ok (ret == -1 && errno == EPERM,
	    "Connect with UDP socket NOT valid. ret=%d, errno=%d: %s",
	    ret, errno, strerror(errno));
	close(fd);

	inet_pton(sin.sin_family, "0.0.0.0", &sin.sin_addr);
	fd = tsocks_libc_socket(sin.sin_family, SOCK_STREAM, 0);
	ret = connect(fd, (struct sockaddr *) &sin, sizeof(sin));
	ok (ret == -1 && errno == EPERM,
	    "Connect with ANY address is NOT valid. ret=%d, errno=%d: %s",
	    ret, errno, strerror(errno));
	close(fd);

	inet_pton(sin.sin_family, "127.0.0.1", &sin.sin_addr);
	fd = tsocks_libc_socket(sin.sin_family, SOCK_STREAM, 0);
	ret = connect(fd, (struct sockaddr *) &sin, sizeof(sin));
	ok (ret == -1 && errno == EPERM,
	    "Connect with local address is NOT valid. ret=%d, errno=%d: %s",
	    ret, errno, strerror(errno));
	close(fd);

	sin6.sin6_family = AF_INET6;
	fd = tsocks_libc_socket(sin6.sin6_family, SOCK_DGRAM, 0);
	ret = connect(fd, (struct sockaddr *) &sin6, sizeof(sin6));
	ok (ret == -1 && errno == EPERM,
	    "Connect with UDPv6 socket NOT valid. ret=%d, errno=%d: %s",
	    ret, errno, strerror(errno));
	close(fd);

	inet_pton(sin6.sin6_family, "::", &sin6.sin6_addr);
	fd = tsocks_libc_socket(sin6.sin6_family, SOCK_STREAM, 0);
	ret = connect(fd, (struct sockaddr *) &sin6, sizeof(sin6));
	ok (ret == -1 && errno == EPERM,
	    "Connect with ANYv6 address is NOT valid. ret=%d, errno=%d: %s",
	    ret, errno, strerror(errno));
	close(fd);

	inet_pton(sin6.sin6_family, "::1", &sin6.sin6_addr);
	fd = tsocks_libc_socket(sin6.sin6_family, SOCK_STREAM, 0);
	ret = connect(fd, (struct sockaddr *) &sin6, sizeof(sin6));
	ok (ret == -1 && errno == EPERM,
	    "Connect with local v6 address is NOT valid. ret=%d, errno=%d: %s",
	    ret, errno, strerror(errno));
	close(fd);

	/* Bad fd. */
	ret = connect(42, (struct sockaddr *) &sin, 42);
	ok (ret == -1 && errno == EBADF,
	    "Bad socket FD. ret=%d errno=%d: %s",
	    ret, errno, strerror(errno));
}

int main(int argc, char **argv)
{
	/* Libtap call for the number of tests planned. */
	plan_tests(NUM_TESTS);

	test_connect_deny();

	return exit_status();
}
