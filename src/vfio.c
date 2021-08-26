/*
 * Copyright (c) 2021 Alyssa Ross <hi@alyssa.is>
 * Copyright (c) 2021 The strace developers.
 * All rights reserved.
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "defs.h"
#include <linux/vfio.h>
#include "xlat/vfio_iommu_map_flags.h"

struct vfio_priv_data {
	bool iommu_type1;
};

int
vfio_ioctl_decode_command_number(int code,
				 struct tcb *tcp, int fd, struct fd_priv_data *fd_data)
{
	if (fd_data
	    && fd_data->miscdev_name
	    && strcmp(fd_data->miscdev_name, "vfio") == 0) {
		char *device = get_fdinfo(tcp->pid, fd, "vfio-driver:\t");
		if (!device)
			return 0;
		struct vfio_priv_data *priv = xmalloc(sizeof(struct vfio_priv_data));
		priv->iommu_type1 = false;
		set_tcb_priv_data(tcp, priv, free,
				  vfio_ioctl_decode_command_number);
		if (strcmp(device, "vfio-iommu-type1") == 0) {
			priv->iommu_type1 = true;
			switch (code) {
			case VFIO_IOMMU_MAP_DMA:
				free(device);
				tprints("VFIO_IOMMU_MAP_DMA");
				return IOCTL_NUMBER_STOP_LOOKUP;
			}
		}
		free(device);
	}
	return 0;
}

static int
vfio_device_ioctl(struct tcb *const tcp, const unsigned int code,
		  const kernel_ulong_t arg)
{
	return RVAL_DECODED;
}

static int
vfio_iommu_ioctl(struct tcb *const tcp, const unsigned int code,
		 const kernel_ulong_t arg)
{
	switch (code) {
	case VFIO_IOMMU_MAP_DMA: {
		struct vfio_iommu_type1_dma_map map;
		size_t minsz = offsetofend(typeof(map), size);

		tprint_arg_next();
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
		PRINT_FIELD_PTR(map, iova);

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
vfio_ioctl(struct tcb *const tcp, const unsigned int code,
	   const kernel_ulong_t arg)
{
	if (!verbose(tcp))
		return RVAL_DECODED;

	if (code >= VFIO_IOMMU_GET_INFO) {
		struct vfio_priv_data *priv = get_tcb_priv_data(tcp,
								vfio_ioctl_decode_command_number);
		if (priv && priv->iommu_type1)
			return vfio_iommu_ioctl(tcp, code, arg);

		return vfio_device_ioctl(tcp, code, arg);
	}

	return RVAL_IOCTL_DECODED;
}
