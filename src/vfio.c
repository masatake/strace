/*
 * Copyright (c) 2021 Alyssa Ross <hi@alyssa.is>
 * Copyright (c) 2025 The strace developers.
 * All rights reserved.
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "defs.h"

#include <linux/vfio.h>

#include "xlat/vfio_type1_vfio_iommu.h"
#include "xlat/vfio_iommu_vfio_powerpc.h"
#include "xlat/vfio_iommu_map_flags.h"

bool
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

static int
vfio_iommu_ioctl(struct tcb *const tcp, const unsigned int code,
		 const kernel_ulong_t arg)
{
	switch (code) {
	case VFIO_IOMMU_MAP_DMA: {
		struct vfio_iommu_type1_dma_map map;
		size_t minsz = offsetofend(typeof(map), size);

		tprints_arg_next_name("argp");
		if (umove_or_printaddr(tcp, arg, &map))
			break;

		if (map.argsz < minsz) {
			printaddr(arg);
			break;
		}

		tprint_struct_begin();

		PRINT_FIELD_U(map, argsz);

		tprint_struct_next();
		PRINT_FIELD_FLAGS(map, flags, vfio_iommu_map_flags,
				  "VFIO_IOMMU_MAP_FLAG_???");

		tprint_struct_next();
		PRINT_FIELD_PTR(map, vaddr);

		tprint_struct_next();
		PRINT_FIELD_X(map, iova);

		tprint_struct_next();
		PRINT_FIELD_U64(map, size);

		if (map.argsz > sizeof map)
			print_nonzero_bytes(tcp, tprint_struct_next,
					    arg, sizeof map,
					    MIN(map.argsz, get_pagesize()),
					    QUOTE_FORCE_HEX);

		tprint_struct_end();
		break;
	}

	default:
		return RVAL_DECODED;
	}

	return RVAL_IOCTL_DECODED;
}

int
vfio_ioctl(struct tcb *const tcp, const struct finfo *finfo,
	   unsigned int code, kernel_ulong_t arg)
{
	if (!verbose(tcp))
		return RVAL_DECODED;

#ifndef POWERPC
	if (code == VFIO_IOMMU_MAP_DMA
	    && vfio_is_misc_vfio_dev(finfo))
		return vfio_iommu_ioctl(tcp, code, arg);
#endif

	return RVAL_DECODED;
}
