/*
 * Copyright (c) 2025 The strace developers.
 * All rights reserved.
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "defs.h"

#include "xlat/vfio_type1_vfio_iommu.h"
#include "xlat/vfio_iommu_vfio_powerpc.h"

static bool
vfio_is_misc_vfio_dev(const struct finfo *finfo)
{
	return (finfo
		&& finfo->type == FINFO_DEV_CHR
		&& finfo->dev.major == 10
		&& finfo->dev.minor == 196);
}

int
vfio_ioctl_decode_command_number(struct tcb *tcp,
				 const struct finfo *finfo,
				 unsigned int code)
{
	if (vfio_is_misc_vfio_dev(finfo)) {
		const char *str = xlookup(
#ifdef POWERPC
			vfio_iommu_vfio_powerpc
#else
			vfio_type1_vfio_iommu
#endif
			, code);
		if (str) {
			tprints_string(str);
			return IOCTL_NUMBER_STOP_LOOKUP;
		}
	}

	return 0;
}
