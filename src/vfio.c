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
#include "xlat/vfio_group_flags.h"
#include "xlat/vfio_iommu_type.h"
#include "xlat/vfio_device_flags.h"
#include "xlat/vfio_region_info_flags.h"
#include "xlat/vfio_irq_info.h"
#include "xlat/vfio_irq_set.h"

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

static int
vfio_device_ioctl(struct tcb *const tcp, const unsigned int code,
		  const kernel_ulong_t arg)
{
	int rval_extra = 0;

	switch (code) {

		/* TODO: VFIO_GET_API_VERSION */
		/* TODO: VFIO_CHECK_EXTENSION */

	case VFIO_SET_IOMMU: {
		tprints_arg_next_name("argp");
		printxval_d(vfio_iommu_type, (int)arg, NULL);

		break;
	}


	case VFIO_GROUP_GET_STATUS: {
		if (entering(tcp))
			return 0;

		struct vfio_group_status group_status;

		tprints_arg_next_name("argp");
		if (umove_or_printaddr(tcp, arg, &group_status))
			break;
		tprint_struct_begin();
		PRINT_FIELD_U(group_status, argsz);
		tprint_struct_next();
		PRINT_FIELD_FLAGS(group_status, flags, vfio_group_flags,
				  "VFIO_GROUP_FLAGS_???");
		tprint_struct_end();

		break;
	}

	case VFIO_GROUP_SET_CONTAINER: {
		int32_t container_fd;

		tprints_arg_next_name("argp");
		if (umove_or_printaddr(tcp, arg, &container_fd))
			break;
		printfd(tcp, container_fd);

		break;
	}

		/* TODO: VFIO_GROUP_UNSET_CONTAINER */

	case VFIO_GROUP_GET_DEVICE_FD: {
		if (entering(tcp)) {
			tprints_arg_next_name("argp");
			printpath(tcp, arg);
			return 0;
		}

		rval_extra = RVAL_FD;
		break;
	}

	case VFIO_DEVICE_GET_INFO: {
		struct vfio_device_info device_info;

		if (entering(tcp)) {
			tprints_arg_next_name("argp");
			if (umove_or_printaddr(tcp, arg, &device_info))
				break;
			tprint_struct_begin();
			PRINT_FIELD_U(device_info, argsz);
			tprint_struct_end();
			return 0;
		}

		if (syserror(tcp) || umove(tcp, arg, &device_info))
			break;

		tprint_value_changed();
		tprint_struct_begin();
		PRINT_FIELD_U(device_info, argsz);
		tprint_struct_next();
		PRINT_FIELD_FLAGS(device_info, flags, vfio_device_flags,
				  "VFIO_DEVICE_FLAGS_???");
		tprint_struct_next();
		PRINT_FIELD_U(device_info, num_regions);
		tprint_struct_next();
		PRINT_FIELD_U(device_info, num_irqs);
		tprint_struct_next();
		PRINT_FIELD_U(device_info, cap_offset);
		/* pad? */
		/* TODO: capabilities */
		tprint_struct_end();

		break;
	}

	case VFIO_DEVICE_GET_REGION_INFO: {
		struct vfio_region_info region_info;

		if (entering(tcp)) {
			tprints_arg_next_name("argp");
			if (umove_or_printaddr(tcp, arg, &region_info))
				break;
			tprint_struct_begin();
			PRINT_FIELD_U(region_info, argsz);
			tprint_struct_next();
			/* TODO: decode VFIO_PCI_BAR0_REGION_INDEX... */
			PRINT_FIELD_U(region_info, index);
			tprint_struct_end();
			return 0;
		}

		if (syserror(tcp) || umove(tcp, arg, &region_info))
			break;

		tprint_value_changed();
		tprint_struct_begin();
		PRINT_FIELD_U(region_info, argsz);
		tprint_struct_next();
		PRINT_FIELD_FLAGS(region_info, flags, vfio_region_info_flags,
				  "VFIO_REGION_INFO_FLAGS_???");
		tprint_struct_next();
		/* TODO: decode VFIO_PCI_BAR0_REGION_INDEX... */
		PRINT_FIELD_U(region_info, index);
		tprint_struct_next();
		PRINT_FIELD_0X(region_info, cap_offset);
		tprint_struct_next();
		PRINT_FIELD_U(region_info, size);
		tprint_struct_next();
		PRINT_FIELD_0X(region_info, offset);
		/* TODO: capabilities */
		tprint_struct_end();

		break;
	}

	case VFIO_DEVICE_GET_IRQ_INFO: {
		if (entering(tcp))
			return 0;

		struct vfio_irq_info irq_info;
		tprints_arg_next_name("argp");
		if (umove_or_printaddr(tcp, arg, &irq_info))
			break;
		tprint_struct_begin();
		PRINT_FIELD_U(irq_info, argsz);
		tprint_struct_next();
		PRINT_FIELD_FLAGS(irq_info, flags, vfio_irq_info,
				  "VFIO_IRQ_INFO_???");
		tprint_struct_next();
		/* TODO: VFIO_PCI_INTX_IRQ_INDEX */
		PRINT_FIELD_U(irq_info, index);
		tprint_struct_next();
		PRINT_FIELD_U(irq_info, count);
		tprint_struct_end();

		break;
	}

	case VFIO_DEVICE_SET_IRQS: {
		struct vfio_irq_set irq_set;
		tprints_arg_next_name("argp");
		if (umove_or_printaddr(tcp, arg, &irq_set))
			break;

		tprint_struct_begin();
		PRINT_FIELD_U(irq_set, argsz);
		tprint_struct_next();
		PRINT_FIELD_FLAGS(irq_set, flags, vfio_irq_set,
				  "VFIO_IRQ_SET_???");
		tprint_struct_next();
		/* TODO: VFIO_PCI_MSIX_IRQ_INDEX,... */
		PRINT_FIELD_U(irq_set, index);
		tprint_struct_next();
		PRINT_FIELD_U(irq_set, start);
		tprint_struct_next();
		PRINT_FIELD_U(irq_set, count);
		if (irq_set.argsz > sizeof(irq_set)) {
			tprint_struct_next();
			tprints_field_name("data");
			const kernel_ulong_t start_addr = arg + offsetof(struct vfio_irq_set, data);
			unsigned int data_type = VFIO_IRQ_SET_DATA_TYPE_MASK & irq_set.flags;
			uint32_t total_data_size = irq_set.argsz - offsetof(struct vfio_irq_set, data);
			kernel_ulong_t nargs = total_data_size / ((data_type == VFIO_IRQ_SET_DATA_EVENTFD)
								  ? sizeof(int32_t)
								  : sizeof (typeof(irq_set.data[0])));

			if (data_type == VFIO_IRQ_SET_DATA_EVENTFD) {
				int32_t fd;
				print_array(tcp, start_addr, nargs,
					    &fd, sizeof(fd), tfetch_mem,
					    print_fd_array_member, NULL);
			} else {
				uint8_t b;
				print_array(tcp, start_addr, nargs,
					    &b, sizeof(b), tfetch_mem,
					    print_uint_array_member, NULL);
			}
		}
		tprint_struct_end();
		break;
	}

		/* TODO: VFIO_DEVICE_RESET
		 * ... */

	default:
		return RVAL_DECODED;
	}

	return RVAL_IOCTL_DECODED | rval_extra;
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

	if (code >= VFIO_GET_API_VERSION)
		return vfio_device_ioctl(tcp, code, arg);

	return RVAL_DECODED;
}
