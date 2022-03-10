/*
 * Test the code resolving the overwrapped IOCTL commands
 *
 * Copyright (c) 2022 The strace developers.
 * All rights reserved.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "tests.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <asm/ioctls.h>
#include <linux/soundcard.h>
#include <linux/limits.h>

#include <errno.h>

int
main(int argc, char **argv)
{
	skip_if_unavailable("/proc/self/fd/");

#if SNDCTL_TMR_START == TCSETS
	struct termios tos;
	memset(&tos, 0, sizeof(tos));

	int ptm = posix_openpt(O_RDWR);
	if (ptm < 0)
		perror_msg_and_skip("posix_openpt(O_RDWR)");

	int unlock = 0;
	if (ioctl(ptm, TIOCSPTLCK, &unlock) < 0)
		perror_msg_and_skip("ioctl(%d, TIOCSPTLCK, ...)", ptm);
	printf("ioctl(%d, TIOCSPTLCK, [0]) = 0\n", ptm);

	int slvnum;
	if (ioctl(ptm, TIOCGPTN, &slvnum) < 0)
		error_msg_and_skip("ioctl(%d, TIOCGPTN, ...)", ptm);
	printf("ioctl(%d, TIOCGPTN, [%d]) = 0\n", ptm, slvnum);

	char slvdev[PATH_MAX];
	snprintf(slvdev, sizeof(slvdev), "/dev/pts/%d", slvnum);

	int pts = open(slvdev, O_RDWR);
	if (pts < 0)
		perror_msg_and_fail("open(%s)", slvdev);

	if (ioctl(pts, TCSETS, &tos) < 0)
		perror_msg_and_fail("ioctl(%d, TCSETS, ...)", pts);
	printf("ioctl(%d, TCSETS, {c_iflag=, c_oflag=NL0|CR0|TAB0|BS0|VT0|FF0|, c_cflag=B0|CS5|, c_lflag=, ...}) = 0\n", pts);

	int devnull = open("/dev/null", O_RDWR);
	if (pts < 0)
		perror_msg_and_fail("open(/dev/null)");

	ioctl(devnull, TCSETS, &tos);
	printf("ioctl(%d, SNDCTL_TMR_START or TCSETS, {c_iflag=, c_oflag=NL0|CR0|TAB0|BS0|VT0|FF0|, c_cflag=B0|CS5|, c_lflag=, ...}) = %s\n",
	       devnull, sprintrc(-1));

	ioctl(-1, TCSETS, &tos);
	printf("ioctl(-1, SNDCTL_TMR_START or TCSETS, {c_iflag=, c_oflag=NL0|CR0|TAB0|BS0|VT0|FF0|, c_cflag=B0|CS5|, c_lflag=, ...}) = %s\n",
	       sprintrc(-1));

	puts("+++ exited with 0 +++");
	return 0;
#else
	error_msg_and_skip("SNDCTL_TMR_START is not equal to TCSETS", ptm);
#endif
}
