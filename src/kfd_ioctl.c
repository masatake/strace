/*
 * Copyright (c) 2023 Hongren (Zenithal) Zheng <i@zenithal.me>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "defs.h"
#include <linux/kfd_ioctl.h>

static int
print_amdkfd_ioc_get_version(struct tcb *const tcp,
	const kernel_ulong_t arg)
{
	struct kfd_ioctl_get_version_args args;

	if (entering(tcp))
		return 0;

	tprint_arg_next();
	if (umove_or_printaddr(tcp, arg, &args))
		return RVAL_IOCTL_DECODED;

	tprint_struct_begin();
	PRINT_FIELD_U(args, major_version);
	tprint_struct_next();
	PRINT_FIELD_U(args, minor_version);
	tprint_struct_end();

	return RVAL_IOCTL_DECODED;
}

#include "xlat/kfd_queue_type.h"

static int
print_amdkfd_ioc_create_queue(struct tcb *const tcp,
	const kernel_ulong_t arg)
{
	struct kfd_ioctl_create_queue_args args;

	if (entering(tcp))
		tprint_arg_next();
	else if (syserror(tcp))
		return RVAL_IOCTL_DECODED;
	else
		tprint_value_changed();

	if (umove_or_printaddr(tcp, arg, &args))
		return RVAL_IOCTL_DECODED;

	if (exiting(tcp)) {
		tprint_struct_begin();
		PRINT_FIELD_X(args, write_pointer_address);
		tprint_struct_next();
		PRINT_FIELD_X(args, read_pointer_address);
		tprint_struct_next();
		PRINT_FIELD_X(args, doorbell_offset);
		tprint_struct_next();
		PRINT_FIELD_X(args, queue_id);
		tprint_struct_end();
		return RVAL_IOCTL_DECODED;
	}

	/* entering */
	tprint_struct_begin();
	PRINT_FIELD_X(args, ring_base_address);
	tprint_struct_next();

	PRINT_FIELD_X(args, ring_size);
	tprint_struct_next();
	PRINT_FIELD_X(args, gpu_id);
	tprint_struct_next();
	PRINT_FIELD_XVAL(args, queue_type, kfd_queue_type, "KFD_IOC_QUEUE_TYPE_???");
	tprint_struct_next();
	PRINT_FIELD_U(args, queue_percentage);
	tprint_struct_next();
	PRINT_FIELD_U(args, queue_priority);
	tprint_struct_next();

	PRINT_FIELD_X(args, eop_buffer_address);
	tprint_struct_next();
	PRINT_FIELD_X(args, eop_buffer_size);
	tprint_struct_next();
	PRINT_FIELD_X(args, ctx_save_restore_address);
	tprint_struct_next();
	PRINT_FIELD_X(args, ctx_save_restore_size);
	tprint_struct_next();
	PRINT_FIELD_X(args, ctl_stack_size);
	tprint_struct_end();
	return 0;
}

static int
print_amdkfd_ioc_destroy_queue(struct tcb *const tcp,
	const kernel_ulong_t arg)
{
	struct kfd_ioctl_destroy_queue_args args;

	tprint_arg_next();
	if (!umove_or_printaddr(tcp, arg, &args)) {
		tprint_struct_begin();
		PRINT_FIELD_X(args, queue_id);
		tprint_struct_end();
	}

	return RVAL_IOCTL_DECODED;
}

#include "xlat/kfd_cache_policy.h"

static int
print_amdkfd_ioc_set_memory_policy(struct tcb *const tcp,
	const kernel_ulong_t arg)
{
	struct kfd_ioctl_set_memory_policy_args args;

	tprint_arg_next();
	if (!umove_or_printaddr(tcp, arg, &args)) {
		tprint_struct_begin();
		PRINT_FIELD_X(args, alternate_aperture_base);
		tprint_struct_next();
		PRINT_FIELD_X(args, alternate_aperture_size);
		tprint_struct_next();

		PRINT_FIELD_X(args, gpu_id);
		tprint_struct_next();
		PRINT_FIELD_XVAL(args, default_policy, kfd_cache_policy,
			"KFD_IOC_CACHE_POLICY_???");
		tprint_struct_next();
		PRINT_FIELD_XVAL(args, alternate_policy, kfd_cache_policy,
			"KFD_IOC_CACHE_POLICY_???");
		tprint_struct_end();
	}

	return RVAL_IOCTL_DECODED;
}

static int
print_amdkfd_ioc_get_clock_counters(struct tcb *const tcp,
	const kernel_ulong_t arg)
{
	struct kfd_ioctl_get_clock_counters_args args;

	if (entering(tcp))
		tprint_arg_next();
	else if (syserror(tcp))
		return RVAL_IOCTL_DECODED;
	else
		tprint_value_changed();

	if (umove_or_printaddr(tcp, arg, &args))
		return RVAL_IOCTL_DECODED;

	if (entering(tcp)) {
		tprint_struct_begin();
		PRINT_FIELD_X(args, gpu_id);
		tprint_struct_end();
		return 0;
	}

	/* exiting */
	tprint_struct_begin();
	PRINT_FIELD_X(args, gpu_clock_counter);
	tprint_struct_next();
	PRINT_FIELD_X(args, cpu_clock_counter);
	tprint_struct_next();
	PRINT_FIELD_X(args, system_clock_counter);
	tprint_struct_next();
	PRINT_FIELD_U(args, system_clock_freq);
	tprint_struct_end();
	return RVAL_IOCTL_DECODED;
}

static int
print_amdkfd_ioc_get_process_apertures(struct tcb *const tcp,
	const kernel_ulong_t arg)
{
	struct kfd_ioctl_get_process_apertures_args args;

	if (entering(tcp))
		return 0;

	tprint_arg_next();
	if (umove_or_printaddr(tcp, arg, &args))
		return RVAL_IOCTL_DECODED;

	tprint_struct_begin();
	PRINT_FIELD_PTR(args, process_apertures); /* TODO: print array */
	tprint_struct_next();
	PRINT_FIELD_U(args, num_of_nodes);
	tprint_struct_end();

	return RVAL_IOCTL_DECODED;
}

static int
print_amdkfd_ioc_update_queue(struct tcb *const tcp,
	const kernel_ulong_t arg)
{
	struct kfd_ioctl_update_queue_args args;

	tprint_arg_next();
	if (!umove_or_printaddr(tcp, arg, &args)) {
		tprint_struct_begin();
		PRINT_FIELD_X(args, ring_base_address);
		tprint_struct_next();

		PRINT_FIELD_X(args, queue_id);
		tprint_struct_next();
		PRINT_FIELD_X(args, ring_size);
		tprint_struct_next();
		PRINT_FIELD_U(args, queue_percentage);
		tprint_struct_next();
		PRINT_FIELD_U(args, queue_priority);
		tprint_struct_end();
	}

	return RVAL_IOCTL_DECODED;
}

#include "xlat/kfd_event_type.h"

static int
print_amdkfd_ioc_create_event(struct tcb *const tcp,
	const kernel_ulong_t arg)
{
	struct kfd_ioctl_create_event_args args;

	if (entering(tcp))
		tprint_arg_next();
	else if (syserror(tcp))
		return RVAL_IOCTL_DECODED;
	else
		tprint_value_changed();

	if (umove_or_printaddr(tcp, arg, &args))
		return RVAL_IOCTL_DECODED;

	if (exiting(tcp)) {
		tprint_struct_begin();
		PRINT_FIELD_X(args, event_page_offset);
		tprint_struct_next();
		PRINT_FIELD_X(args, event_trigger_data);
		tprint_struct_next();
		PRINT_FIELD_X(args, event_id);
		tprint_struct_next();
		PRINT_FIELD_X(args, event_slot_index);
		tprint_struct_end();
		return RVAL_IOCTL_DECODED;
	}

	/* entering */
	tprint_struct_begin();
	PRINT_FIELD_XVAL(args, event_type, kfd_event_type, "KFD_IOC_EVENT_TYPE_???");
	tprint_struct_next();
	PRINT_FIELD_X(args, auto_reset);
	tprint_struct_next();
	PRINT_FIELD_X(args, node_id);
	tprint_struct_end();
	return 0;
}

static int
print_amdkfd_ioc_destroy_event(struct tcb *const tcp,
	const kernel_ulong_t arg)
{
	struct kfd_ioctl_destroy_event_args args;

	tprint_arg_next();
	if (!umove_or_printaddr(tcp, arg, &args)) {
		tprint_struct_begin();
		PRINT_FIELD_X(args, event_id);
		tprint_struct_end();
	}

	return RVAL_IOCTL_DECODED;
}

static int
print_amdkfd_ioc_set_event(struct tcb *const tcp,
	const kernel_ulong_t arg)
{
	struct kfd_ioctl_set_event_args args;

	tprint_arg_next();
	if (!umove_or_printaddr(tcp, arg, &args)) {
		tprint_struct_begin();
		PRINT_FIELD_X(args, event_id);
		tprint_struct_end();
	}

	return RVAL_IOCTL_DECODED;
}

static int
print_amdkfd_ioc_reset_event(struct tcb *const tcp,
	const kernel_ulong_t arg)
{
	struct kfd_ioctl_reset_event_args args;

	tprint_arg_next();
	if (!umove_or_printaddr(tcp, arg, &args)) {
		tprint_struct_begin();
		PRINT_FIELD_X(args, event_id);
		tprint_struct_end();
	}

	return RVAL_IOCTL_DECODED;
}

#include "xlat/kfd_wait_result.h"

static int
print_amdkfd_ioc_wait_events(struct tcb *const tcp,
	const kernel_ulong_t arg)
{
	struct kfd_ioctl_wait_events_args args;

	if (entering(tcp))
		tprint_arg_next();
	else if (syserror(tcp))
		return RVAL_IOCTL_DECODED;
	else
		tprint_value_changed();

	if (umove_or_printaddr(tcp, arg, &args))
		return RVAL_IOCTL_DECODED;

	if (exiting(tcp)) {
		tprint_struct_begin();
		PRINT_FIELD_XVAL(args, wait_result, kfd_wait_result, "KFD_IOC_WAIT_RESULT_???");
		tprint_struct_end();
		return RVAL_IOCTL_DECODED;
	}

	/* entering */
	tprint_struct_begin();
	PRINT_FIELD_PTR(args, events_ptr);
	tprint_struct_next();
	PRINT_FIELD_X(args, num_events);
	tprint_struct_next();
	PRINT_FIELD_X(args, wait_for_all);
	tprint_struct_next();
	PRINT_FIELD_U(args, timeout);
	tprint_struct_end();
	return 0;
}

static int
print_amdkfd_ioc_set_scratch_backing_va(struct tcb *const tcp,
	const kernel_ulong_t arg)
{
	struct kfd_ioctl_set_scratch_backing_va_args args;

	tprint_arg_next();
	if (!umove_or_printaddr(tcp, arg, &args)) {
		tprint_struct_begin();
		PRINT_FIELD_X(args, va_addr);
		tprint_struct_next();
		PRINT_FIELD_X(args, gpu_id);
		tprint_struct_end();
	}

	return RVAL_IOCTL_DECODED;
}

static int
print_amdkfd_ioc_get_tile_config(struct tcb *const tcp,
	const kernel_ulong_t arg)
{
	struct kfd_ioctl_get_tile_config_args args;

	if (entering(tcp))
		tprint_arg_next();
	else if (syserror(tcp))
		return RVAL_IOCTL_DECODED;
	else
		tprint_value_changed();

	if (umove_or_printaddr(tcp, arg, &args))
		return RVAL_IOCTL_DECODED;

	if (exiting(tcp)) {
		tprint_struct_begin();
		PRINT_FIELD_U(args, num_tile_configs);
		tprint_struct_next();
		PRINT_FIELD_U(args, num_macro_tile_configs);
		tprint_struct_next();

		PRINT_FIELD_U(args, gb_addr_config);
		tprint_struct_next();
		PRINT_FIELD_U(args, num_banks);
		tprint_struct_next();
		PRINT_FIELD_U(args, num_ranks);
		tprint_struct_end();
		return RVAL_IOCTL_DECODED;
	}

	/* entering */
	tprint_struct_begin();
	PRINT_FIELD_PTR(args, tile_config_ptr);
	tprint_struct_next();
	PRINT_FIELD_PTR(args, macro_tile_config_ptr);
	tprint_struct_next();
	PRINT_FIELD_U(args, num_tile_configs);
	tprint_struct_next();
	PRINT_FIELD_U(args, num_macro_tile_configs);
	tprint_struct_next();

	PRINT_FIELD_X(args, gpu_id);
	tprint_struct_end();
	return 0;
}

static int
print_amdkfd_ioc_set_trap_handler(struct tcb *const tcp,
	const kernel_ulong_t arg)
{
	struct kfd_ioctl_set_trap_handler_args args;

	tprint_arg_next();
	if (!umove_or_printaddr(tcp, arg, &args)) {
		tprint_struct_begin();
		PRINT_FIELD_X(args, tba_addr);
		tprint_struct_next();
		PRINT_FIELD_X(args, tma_addr);
		tprint_struct_next();
		PRINT_FIELD_X(args, gpu_id);
		tprint_struct_end();
	}

	return RVAL_IOCTL_DECODED;
}

static int
print_amdkfd_ioc_get_process_apertures_new(struct tcb *const tcp,
	const kernel_ulong_t arg)
{
	struct kfd_ioctl_get_process_apertures_new_args args;

	if (entering(tcp))
		return 0;

	tprint_arg_next();
	if (umove_or_printaddr(tcp, arg, &args))
		return RVAL_IOCTL_DECODED;

	tprint_struct_begin();
	PRINT_FIELD_PTR(args, kfd_process_device_apertures_ptr); /* TODO: print array */
	tprint_struct_next();
	PRINT_FIELD_U(args, num_of_nodes);
	tprint_struct_end();

	return RVAL_IOCTL_DECODED;
}

static int
print_amdkfd_ioc_acquire_vm(struct tcb *const tcp,
	const kernel_ulong_t arg)
{
	struct kfd_ioctl_acquire_vm_args args;

	tprint_arg_next();
	if (!umove_or_printaddr(tcp, arg, &args)) {
		tprint_struct_begin();
		PRINT_FIELD_FD(args, drm_fd, tcp);
		tprint_struct_next();
		PRINT_FIELD_X(args, gpu_id);
		tprint_struct_end();
	}

	return RVAL_IOCTL_DECODED;
}

#include "xlat/kfd_alloc_mem_flags.h"

static int
print_amdkfd_ioc_alloc_memory_of_gpu(struct tcb *const tcp,
	const kernel_ulong_t arg)
{
	struct kfd_ioctl_alloc_memory_of_gpu_args args;

	if (entering(tcp))
		tprint_arg_next();
	else if (syserror(tcp))
		return RVAL_IOCTL_DECODED;
	else
		tprint_value_changed();

	if (umove_or_printaddr(tcp, arg, &args))
		return RVAL_IOCTL_DECODED;

	if (exiting(tcp)) {
		tprint_struct_begin();
		PRINT_FIELD_X(args, handle);
		tprint_struct_end();
		return RVAL_IOCTL_DECODED;
	}

	/* entering */
	tprint_struct_begin();
	PRINT_FIELD_X(args, va_addr);
	tprint_struct_next();
	PRINT_FIELD_X(args, size);
	tprint_struct_next();
	PRINT_FIELD_X(args, mmap_offset);
	tprint_struct_next();
	PRINT_FIELD_X(args, gpu_id);
	tprint_struct_next();
	PRINT_FIELD_FLAGS(args, flags, kfd_alloc_mem_flags, "KFD_IOC_ALLOC_MEM_FLAGS_???");
	tprint_struct_end();
	return 0;
}

static int
print_amdkfd_ioc_free_memory_of_gpu(struct tcb *const tcp,
	const kernel_ulong_t arg)
{
	struct kfd_ioctl_free_memory_of_gpu_args args;

	tprint_arg_next();
	if (!umove_or_printaddr(tcp, arg, &args)) {
		tprint_struct_begin();
		PRINT_FIELD_X(args, handle);
		tprint_struct_end();
	}

	return RVAL_IOCTL_DECODED;
}

static int
print_amdkfd_ioc_map_memory_to_gpu(struct tcb *const tcp,
	const kernel_ulong_t arg)
{
	struct kfd_ioctl_map_memory_to_gpu_args args;

	if (entering(tcp))
		tprint_arg_next();
	else if (syserror(tcp))
		return RVAL_IOCTL_DECODED;
	else
		tprint_value_changed();

	if (umove_or_printaddr(tcp, arg, &args))
		return RVAL_IOCTL_DECODED;

	if (exiting(tcp)) {
		tprint_struct_begin();
		PRINT_FIELD_U(args, n_success);
		tprint_struct_end();
		return RVAL_IOCTL_DECODED;
	}

	/* entering */
	tprint_struct_begin();
	PRINT_FIELD_X(args, handle);
	tprint_struct_next();
	PRINT_FIELD_PTR(args, device_ids_array_ptr);
	tprint_struct_next();
	PRINT_FIELD_U(args, n_devices);
	tprint_struct_next();
	PRINT_FIELD_U(args, n_success);
	tprint_struct_end();
	return 0;
}

static int
print_amdkfd_ioc_unmap_memory_from_gpu(struct tcb *const tcp,
	const kernel_ulong_t arg)
{
	struct kfd_ioctl_unmap_memory_from_gpu_args args;

	if (entering(tcp))
		tprint_arg_next();
	else if (syserror(tcp))
		return RVAL_IOCTL_DECODED;
	else
		tprint_value_changed();

	if (umove_or_printaddr(tcp, arg, &args))
		return RVAL_IOCTL_DECODED;

	if (exiting(tcp)) {
		tprint_struct_begin();
		PRINT_FIELD_U(args, n_success);
		tprint_struct_end();
		return RVAL_IOCTL_DECODED;
	}

	/* entering */
	tprint_struct_begin();
	PRINT_FIELD_X(args, handle);
	tprint_struct_next();
	PRINT_FIELD_PTR(args, device_ids_array_ptr);
	tprint_struct_next();
	PRINT_FIELD_U(args, n_devices);
	tprint_struct_next();
	PRINT_FIELD_U(args, n_success);
	tprint_struct_end();
	return 0;
}

static int
print_amdkfd_ioc_set_cu_mask(struct tcb *const tcp,
	const kernel_ulong_t arg)
{
	struct kfd_ioctl_set_cu_mask_args args;

	tprint_arg_next();
	if (!umove_or_printaddr(tcp, arg, &args)) {
		tprint_struct_begin();
		PRINT_FIELD_X(args, queue_id);
		tprint_struct_next();
		PRINT_FIELD_U(args, num_cu_mask);
		tprint_struct_next();
		PRINT_FIELD_PTR(args, cu_mask_ptr);
		tprint_struct_end();
	}

	return RVAL_IOCTL_DECODED;
}

static int
print_amdkfd_ioc_get_queue_wave_state(struct tcb *const tcp,
	const kernel_ulong_t arg)
{
	struct kfd_ioctl_get_queue_wave_state_args args;

	if (entering(tcp))
		tprint_arg_next();
	else if (syserror(tcp))
		return RVAL_IOCTL_DECODED;
	else
		tprint_value_changed();

	if (umove_or_printaddr(tcp, arg, &args))
		return RVAL_IOCTL_DECODED;

	if (exiting(tcp)) {
		tprint_struct_begin();
		PRINT_FIELD_X(args, ctl_stack_used_size);
		tprint_struct_next();
		PRINT_FIELD_X(args, save_area_used_size);
		tprint_struct_end();
		return RVAL_IOCTL_DECODED;
	}

	/* entering */
	tprint_struct_begin();
	PRINT_FIELD_X(args, ctl_stack_address);
	tprint_struct_next();
	PRINT_FIELD_U(args, queue_id);
	tprint_struct_end();
	return 0;
}

static int
print_amdkfd_ioc_get_dmabuf_info(struct tcb *const tcp,
	const kernel_ulong_t arg)
{
	struct kfd_ioctl_get_dmabuf_info_args args;

	if (entering(tcp))
		tprint_arg_next();
	else if (syserror(tcp))
		return RVAL_IOCTL_DECODED;
	else
		tprint_value_changed();

	if (umove_or_printaddr(tcp, arg, &args))
		return RVAL_IOCTL_DECODED;

	if (exiting(tcp)) {
		tprint_struct_begin();
		PRINT_FIELD_X(args, size);
		tprint_struct_next();
		PRINT_FIELD_X(args, metadata_size);
		tprint_struct_next();
		PRINT_FIELD_X(args, gpu_id);
		tprint_struct_next();
		PRINT_FIELD_FLAGS(args, flags, kfd_alloc_mem_flags, "KFD_IOC_ALLOC_MEM_FLAGS_???");
		tprint_struct_end();
		return RVAL_IOCTL_DECODED;
	}

	/* entering */
	tprint_struct_begin();
	PRINT_FIELD_PTR(args, metadata_ptr);
	tprint_struct_next();
	PRINT_FIELD_X(args, metadata_size);
	tprint_struct_next();
	PRINT_FIELD_FD(args, dmabuf_fd, tcp);
	tprint_struct_end();
	return 0;
}

static int
print_amdkfd_ioc_import_dmabuf(struct tcb *const tcp,
	const kernel_ulong_t arg)
{
	struct kfd_ioctl_import_dmabuf_args args;

	if (entering(tcp))
		tprint_arg_next();
	else if (syserror(tcp))
		return RVAL_IOCTL_DECODED;
	else
		tprint_value_changed();

	if (umove_or_printaddr(tcp, arg, &args))
		return RVAL_IOCTL_DECODED;

	if (exiting(tcp)) {
		tprint_struct_begin();
		PRINT_FIELD_X(args, handle);
		tprint_struct_end();
		return RVAL_IOCTL_DECODED;
	}

	/* entering */
	tprint_struct_begin();
	PRINT_FIELD_X(args, va_addr);
	tprint_struct_next();
	PRINT_FIELD_X(args, gpu_id);
	tprint_struct_next();
	PRINT_FIELD_FD(args, dmabuf_fd, tcp);
	tprint_struct_end();
	return 0;
}

static int
print_amdkfd_ioc_alloc_queue_gws(struct tcb *const tcp,
	const kernel_ulong_t arg)
{
	struct kfd_ioctl_alloc_queue_gws_args args;

	if (entering(tcp))
		tprint_arg_next();
	else if (syserror(tcp))
		return RVAL_IOCTL_DECODED;
	else
		tprint_value_changed();

	if (umove_or_printaddr(tcp, arg, &args))
		return RVAL_IOCTL_DECODED;

	if (exiting(tcp)) {
		tprint_struct_begin();
		PRINT_FIELD_X(args, first_gws);
		tprint_struct_end();
		return RVAL_IOCTL_DECODED;
	}

	/* entering */
	tprint_struct_begin();
	PRINT_FIELD_X(args, queue_id);
	tprint_struct_next();
	PRINT_FIELD_X(args, num_gws);
	tprint_struct_end();
	return 0;
}

static int
print_amdkfd_ioc_smi_events(struct tcb *const tcp,
	const kernel_ulong_t arg)
{
	struct kfd_ioctl_smi_events_args args;

	if (entering(tcp))
		tprint_arg_next();
	else if (syserror(tcp))
		return RVAL_IOCTL_DECODED;
	else
		tprint_value_changed();

	if (umove_or_printaddr(tcp, arg, &args))
		return RVAL_IOCTL_DECODED;

	if (exiting(tcp)) {
		tprint_struct_begin();
		PRINT_FIELD_FD(args, anon_fd, tcp);
		tprint_struct_end();
		return RVAL_IOCTL_DECODED;
	}

	/* entering */
	tprint_struct_begin();
	PRINT_FIELD_X(args, gpuid);
	tprint_struct_end();
	return 0;
}

#include "xlat/kfd_svm_attr_type.h"
#include "xlat/kfd_svm_location.h"
#include "xlat/kfd_svm_flag.h"

static void
print_kfd_svm_attribute(const struct kfd_ioctl_svm_attribute *attr)
{
	tprint_struct_begin();
	PRINT_FIELD_XVAL(*attr, type, kfd_svm_attr_type, "KFD_IOCTL_SVM_ATTR_???");
	tprint_struct_next();
	switch (attr->type) {
	case KFD_IOCTL_SVM_ATTR_PREFERRED_LOC:
	case KFD_IOCTL_SVM_ATTR_PREFETCH_LOC:
		PRINT_FIELD_XVAL(*attr, value, kfd_svm_location, "KFD_IOCTL_SVM_LOCATION_???");
		break;
	case KFD_IOCTL_SVM_ATTR_SET_FLAGS:
	case KFD_IOCTL_SVM_ATTR_CLR_FLAGS:
		PRINT_FIELD_FLAGS(*attr, value, kfd_svm_flag, "KFD_IOCTL_SVM_FLAG_???");
		break;
	default:
		PRINT_FIELD_X(*attr, value);
	}
	tprint_struct_end();
}

static void
print_kfd_svm_args_attrs(struct tcb *tcp, const kernel_ulong_t arg, size_t nattr)
{
	const kernel_ulong_t attrs = arg + sizeof(struct kfd_ioctl_svm_args);
	struct kfd_ioctl_svm_attribute attr;

	tprints_field_name("attrs");
	tprint_array_begin();
	for (size_t i = 0; i < nattr; ++i) {
		tfetch_obj(tcp, attrs + i * sizeof(struct kfd_ioctl_svm_attribute), &attr);
		print_kfd_svm_attribute(&attr);
	}
	tprint_array_end();
}

#include "xlat/kfd_svm_op.h"

static int
print_amdkfd_ioc_svm(struct tcb *const tcp,
	const kernel_ulong_t arg)
{
	struct kfd_ioctl_svm_args args;

	if (entering(tcp))
		tprint_arg_next();
	else if (syserror(tcp))
		return RVAL_IOCTL_DECODED;

	if (umove_or_printaddr(tcp, arg, &args))
		return RVAL_IOCTL_DECODED;

	if (exiting(tcp)) {
		if (args.op == KFD_IOCTL_SVM_OP_GET_ATTR) {
			tprint_value_changed();
			tprint_struct_begin();
			print_kfd_svm_args_attrs(tcp, arg, args.nattr);
			tprint_struct_end();
		}
		return RVAL_IOCTL_DECODED;
	}

	/* entering */
	tprint_struct_begin();
	PRINT_FIELD_X(args, start_addr);
	tprint_struct_next();
	PRINT_FIELD_X(args, size);
	tprint_struct_next();
	PRINT_FIELD_XVAL(args, op, kfd_svm_op, "KFD_IOCTL_SVM_OP_???");
	tprint_struct_next();
	PRINT_FIELD_U(args, nattr);
	if (args.op == KFD_IOCTL_SVM_OP_SET_ATTR) {
		tprint_struct_next();
		print_kfd_svm_args_attrs(tcp, arg, args.nattr);
	}
	tprint_struct_end();
	return 0;
}

static int
print_amdkfd_ioc_set_xnack_mode(struct tcb *const tcp,
	const kernel_ulong_t arg)
{
	struct kfd_ioctl_set_xnack_mode_args args;

	if (entering(tcp))
		tprint_arg_next();
	else if (syserror(tcp))
		return RVAL_IOCTL_DECODED;
	else
		tprint_value_changed();

	if (umove_or_printaddr(tcp, arg, &args))
		return RVAL_IOCTL_DECODED;

	if (exiting(tcp)) {
		tprint_struct_begin();
		PRINT_FIELD_X(args, xnack_enabled);
		tprint_struct_end();
		return RVAL_IOCTL_DECODED;
	}

	/* entering */
	tprint_struct_begin();
	PRINT_FIELD_X(args, xnack_enabled);
	tprint_struct_end();
	return 0;
}

#include "xlat/kfd_criu_op.h"

static int
print_amdkfd_ioc_criu_op(struct tcb *const tcp,
	const kernel_ulong_t arg)
{
	struct kfd_ioctl_criu_args args;

	if (entering(tcp))
		tprint_arg_next();
	else if (syserror(tcp))
		return RVAL_IOCTL_DECODED;
	else
		tprint_value_changed();

	if (umove_or_printaddr(tcp, arg, &args))
		return RVAL_IOCTL_DECODED;

	tprint_struct_begin();
	PRINT_FIELD_PTR(args, devices);
	tprint_struct_next();
	PRINT_FIELD_PTR(args, bos);
	tprint_struct_next();
	PRINT_FIELD_PTR(args, priv_data);
	tprint_struct_next();
	PRINT_FIELD_X(args, priv_data_size);
	tprint_struct_next();
	PRINT_FIELD_U(args, num_devices);
	tprint_struct_next();
	PRINT_FIELD_U(args, num_bos);
	tprint_struct_next();
	PRINT_FIELD_U(args, num_objects);
	tprint_struct_next();
	PRINT_FIELD_U(args, pid);
	tprint_struct_next();
	if (entering(tcp))
		PRINT_FIELD_XVAL(args, op, kfd_criu_op, "KFD_CRIU_OP_???");
	tprint_struct_end();
	return 0;
}

static int
print_amdkfd_ioc_available_memory(struct tcb *const tcp,
	const kernel_ulong_t arg)
{
	struct kfd_ioctl_get_available_memory_args args;

	if (entering(tcp))
		tprint_arg_next();
	else if (syserror(tcp))
		return RVAL_IOCTL_DECODED;
	else
		tprint_value_changed();

	if (umove_or_printaddr(tcp, arg, &args))
		return RVAL_IOCTL_DECODED;

	if (exiting(tcp)) {
		tprint_struct_begin();
		PRINT_FIELD_X(args, available);
		tprint_struct_end();
		return RVAL_IOCTL_DECODED;
	}

	/* entering */
	tprint_struct_begin();
	PRINT_FIELD_X(args, gpu_id);
	tprint_struct_end();
	return 0;
}

int
kfd_ioctl(struct tcb *const tcp, const unsigned int code,
	const kernel_ulong_t arg)
{
	switch (code) {
	case AMDKFD_IOC_GET_VERSION:
		return print_amdkfd_ioc_get_version(tcp, arg);
	case AMDKFD_IOC_CREATE_QUEUE:
		return print_amdkfd_ioc_create_queue(tcp, arg);
	case AMDKFD_IOC_DESTROY_QUEUE:
		return print_amdkfd_ioc_destroy_queue(tcp, arg);
	case AMDKFD_IOC_SET_MEMORY_POLICY:
		return print_amdkfd_ioc_set_memory_policy(tcp, arg);
	case AMDKFD_IOC_GET_CLOCK_COUNTERS:
		return print_amdkfd_ioc_get_clock_counters(tcp, arg);
	case AMDKFD_IOC_GET_PROCESS_APERTURES:
		return print_amdkfd_ioc_get_process_apertures(tcp, arg);
	case AMDKFD_IOC_UPDATE_QUEUE:
		return print_amdkfd_ioc_update_queue(tcp, arg);
	case AMDKFD_IOC_CREATE_EVENT:
		return print_amdkfd_ioc_create_event(tcp, arg);
	case AMDKFD_IOC_DESTROY_EVENT:
		return print_amdkfd_ioc_destroy_event(tcp, arg);
	case AMDKFD_IOC_SET_EVENT:
		return print_amdkfd_ioc_set_event(tcp, arg);
	case AMDKFD_IOC_RESET_EVENT:
		return print_amdkfd_ioc_reset_event(tcp, arg);
	case AMDKFD_IOC_WAIT_EVENTS:
		return print_amdkfd_ioc_wait_events(tcp, arg);
	/* TODO: DEPRECATED ioctl */
	case AMDKFD_IOC_SET_SCRATCH_BACKING_VA:
		return print_amdkfd_ioc_set_scratch_backing_va(tcp, arg);
	case AMDKFD_IOC_GET_TILE_CONFIG:
		return print_amdkfd_ioc_get_tile_config(tcp, arg);
	case AMDKFD_IOC_SET_TRAP_HANDLER:
		return print_amdkfd_ioc_set_trap_handler(tcp, arg);
	case AMDKFD_IOC_GET_PROCESS_APERTURES_NEW:
		return print_amdkfd_ioc_get_process_apertures_new(tcp, arg);
	case AMDKFD_IOC_ACQUIRE_VM:
		return print_amdkfd_ioc_acquire_vm(tcp, arg);
	case AMDKFD_IOC_ALLOC_MEMORY_OF_GPU:
		return print_amdkfd_ioc_alloc_memory_of_gpu(tcp, arg);
	case AMDKFD_IOC_FREE_MEMORY_OF_GPU:
		return print_amdkfd_ioc_free_memory_of_gpu(tcp, arg);
	case AMDKFD_IOC_MAP_MEMORY_TO_GPU:
		return print_amdkfd_ioc_map_memory_to_gpu(tcp, arg);
	case AMDKFD_IOC_UNMAP_MEMORY_FROM_GPU:
		return print_amdkfd_ioc_unmap_memory_from_gpu(tcp, arg);
	case AMDKFD_IOC_SET_CU_MASK:
		return print_amdkfd_ioc_set_cu_mask(tcp, arg);
	case AMDKFD_IOC_GET_QUEUE_WAVE_STATE:
		return print_amdkfd_ioc_get_queue_wave_state(tcp, arg);
	case AMDKFD_IOC_GET_DMABUF_INFO:
		return print_amdkfd_ioc_get_dmabuf_info(tcp, arg);
	case AMDKFD_IOC_IMPORT_DMABUF:
		return print_amdkfd_ioc_import_dmabuf(tcp, arg);
	case AMDKFD_IOC_ALLOC_QUEUE_GWS:
		return print_amdkfd_ioc_alloc_queue_gws(tcp, arg);
	case AMDKFD_IOC_SMI_EVENTS:
		return print_amdkfd_ioc_smi_events(tcp, arg);
	case AMDKFD_IOC_SVM:
		return print_amdkfd_ioc_svm(tcp, arg);
	case AMDKFD_IOC_SET_XNACK_MODE:
		return print_amdkfd_ioc_set_xnack_mode(tcp, arg);
	case AMDKFD_IOC_CRIU_OP:
		return print_amdkfd_ioc_criu_op(tcp, arg);
	case AMDKFD_IOC_AVAILABLE_MEMORY:
		return print_amdkfd_ioc_available_memory(tcp, arg);
	}
	return RVAL_DECODED;
}
