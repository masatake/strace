/*
 * SubUnwinder backends interface.
 *
 * Copyright (c) 2018 Masatake YAMATO
 * Copyright (c) 2018 Red Hat, Inc.
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

#ifndef STRACE_SUBUNWIND_H
#define STRACE_SUBUNWIND_H

#include "defs.h"
#include "unwind.h"
#include <elfutils/libdwfl.h>

/*
 * Type used in stacktrace walker.
 */

struct subunwind_unwinder_t;
struct subunwind_context_t {
	struct subunwind_unwinder_t *unwinder;
};

struct subunwind_unwinder_t {
	const char *name;

	struct subunwind_context_t * (*probe)(struct subunwind_unwinder_t *,
					      Dwfl_Module *,
					      const char *,
					      const char *);
	bool   (*walk)(struct subunwind_context_t *,
		       struct tcb *tcp,
		       Dwfl_Module *,
		       const char *,
		       GElf_Sym,
		       const char *,
		       Dwarf_Addr,
		       unwind_call_action_fn,
		       unwind_error_action_fn,
		       void *);
	void   (*finish)(struct subunwind_context_t *);
};

#endif /* !STRACE_SUBUNWIND_H */
