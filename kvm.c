/*
 * Support for decoding of KVM_* ioctl commands.
 *
 * Copyright (c) 2017 Masatake YAMATO <yamato@redhat.com>
 * Copyright (c) 2017 Red Hat, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "defs.h"

#ifdef HAVE_LINUX_KVM_H
#include <linux/kvm.h>

static int
kvm_ioctl_irq_line_status(struct tcb *const tcp, const kernel_ulong_t arg)
{
	struct kvm_irq_level *irq_level;
	struct kvm_irq_level irq_level_rec;
	struct kvm_irq_level *entering_irq_level;

	if (entering(tcp)) {
		irq_level = malloc(sizeof(*irq_level));
		if (!irq_level)
			return 0;
	} else {
		irq_level = &irq_level_rec;
	}

	if (umoven(tcp, arg, sizeof(*irq_level), irq_level) < 0) {
		if (entering(tcp))
			free(irq_level);
		return 0;
	}
	if (entering(tcp)) {
		set_tcb_priv_data(tcp, irq_level, free);
		return 0;
	}

	entering_irq_level = get_tcb_priv_data(tcp);

	if (entering_irq_level) {
		if (syserror(tcp))
			tprintf(", {{irq=%u}, level=%u}",
				entering_irq_level->irq,
				irq_level->level);
		else
			tprintf(", {{irq=%u, status=%d}, level=%u}",
				entering_irq_level->irq, irq_level->status,
				irq_level->level);
	} else {
		if (syserror(tcp))
			tprintf(", {{irq=%u}, level=%u}",
				irq_level->irq,
				irq_level->level);
		else
			tprintf(", {{irq=???, status=%d}, level=%u}",
				irq_level->status, irq_level->level);
	}
	return RVAL_IOCTL_DECODED;
}

static int
kvm_ioctl_get_dirty_log(struct tcb *const tcp, const kernel_ulong_t arg)
{
	struct kvm_dirty_log dirty_log;

	if (umoven(tcp, arg, sizeof(dirty_log), &dirty_log) < 0)
		return RVAL_DECODED;

	tprintf(", {slot=%d, dirty_bitmap=%p}",
		dirty_log.slot, dirty_log.dirty_bitmap);

	return RVAL_IOCTL_DECODED;
}

int
kvm_ioctl(struct tcb *const tcp, const unsigned int code, const kernel_ulong_t arg)
{
	switch (code) {
	case KVM_RUN:
		/* Generic ioctl decoder is enough for
		   KVM_RUN because it takes no argument. */
		return RVAL_DECODED;
	case KVM_IRQ_LINE_STATUS:
		return kvm_ioctl_irq_line_status(tcp, arg);
	case KVM_GET_DIRTY_LOG:
		return kvm_ioctl_get_dirty_log(tcp, arg);
	default:
		return RVAL_DECODED;
	}
}
#else
int
kvm_ioctl(struct tcb *const tcp, const unsigned int code, const kernel_ulong_t arg)
{
	return RVAL_DECODED;
}
#endif /* HAVE_LINUX_KVM_H */
