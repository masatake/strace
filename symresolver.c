/*
 * Copyright (c) 2020 The strace developers.
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "defs.h"

#if !defined(ENABLE_STACKTRACE) || !defined(USE_LIBDW)
const char *
resolve_symbol_name(struct tcb *tcp, kernel_ulong_t handler)
{
	return NULL;
}
#endif
