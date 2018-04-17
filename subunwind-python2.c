/*
 * python2 subunwinder
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

#include "defs.h"
#include "subunwind.h"
#include "config.h"

#include <string.h>
#include <stddef.h>

#include <Python.h>
#include <frameobject.h>

struct subunwind_python2_context_t {
	struct subunwind_context_t base;
	char *expected_modname;
	GElf_Addr thread_state_addr;
};

static struct subunwind_context_t *
subunwind_python2_probe(struct subunwind_unwinder_t *unwinder,
			Dwfl_Module *mod,
			const char *modname,
			const char *symname)
{
	struct subunwind_python2_context_t *ctx;
	static char *expected_modname;

	if (strcmp (symname, "PyEval_EvalFrameEx"))
		return NULL;

	if (!expected_modname) {
		char *tmp = strstr (PYTHON2_LIBS, "-lpython");
		if (tmp == NULL)
			return NULL;

		tmp += 2;
		expected_modname = xstrdup (tmp);
		tmp = strchr (expected_modname, ' ');
		if (tmp)
			*tmp = '\0';
	}

	if (strstr (modname, expected_modname) == NULL)
		return NULL;
		
	ctx = xmalloc (sizeof (struct subunwind_python2_context_t));
	ctx->base.unwinder = unwinder;
	ctx->expected_modname = xstrdup(expected_modname);
	ctx->thread_state_addr = 0;
	
	int nsyms = dwfl_module_getsymtab (mod);
	for (int i = dwfl_module_getsymtab_first_global(mod);
	     i < nsyms;
	     i++)
	{
		GElf_Sym sym;
		GElf_Addr addr;
		const char *s = dwfl_module_getsym_info (mod, i, &sym,
							 &addr, NULL,
							 NULL, NULL);
		if (strcmp (s, "_PyThreadState_Current") == 0)
		{
			ctx->thread_state_addr = addr;
			break;
		}
	}

	return (struct subunwind_context_t *)ctx;
}

static bool
subunwind_python2_walk(struct subunwind_context_t *ctx,
		       struct tcb *tcp,
		       Dwfl_Module *mod,
		       const char* modname,
		       GElf_Sym sym,
		       const char* symname,
		       Dwarf_Addr bias,
		       unwind_call_action_fn call_action,
		       unwind_error_action_fn error_action,
		       void * action_data)
{
	struct subunwind_python2_context_t *python2_ctx
		= (struct subunwind_python2_context_t *)ctx;

	if (!python2_ctx->thread_state_addr)
		return false;
	if (strstr (modname, python2_ctx->expected_modname) == NULL)
		return false;
	if (strcmp (symname, "PyEval_EvalFrameEx"))
		return false;

	kernel_ulong_t state = 0;
	if (umove(tcp, python2_ctx->thread_state_addr, &state) < 0)
		return false;
	if (!state)
		return false;
	kernel_ulong_t frame;
	if (umove(tcp, state + offsetof(PyThreadState, frame), &frame) < 0)
		return false;
	
	do {
		kernel_ulong_t f_code;
		if (umove(tcp, frame + offsetof(PyFrameObject, f_code), &f_code) < 0)
			break;

		kernel_ulong_t co_filename;
		if (umove(tcp, f_code + offsetof(PyCodeObject, co_filename), &co_filename) < 0)
			break;
		kernel_ulong_t filename_size;
		if (umove(tcp, co_filename + offsetof(PyStringObject, ob_size),
			  &filename_size) < 0)
			break;

		unsigned int prefix_len = 1 + strlen (ctx->unwinder->name) + 1;
		char *filename = xmalloc(prefix_len +
					 filename_size + 1);
		filename [0] = '<';
		strcpy (filename + 1, ctx->unwinder->name);
		filename [prefix_len - 1] = '>';
		filename [prefix_len + filename_size] = '\0';
		if (umovestr(tcp, co_filename + offsetof(PyStringObject, ob_sval),
			     filename_size, filename + prefix_len) < 0) {
			free(filename);
			break;
		}
		kernel_ulong_t co_name;
		if (umove(tcp, f_code + offsetof(PyCodeObject, co_name), &co_name) < 0) {
			free (filename);
			break;
		}
		kernel_ulong_t name_size;
		if (umove(tcp, co_name + offsetof(PyStringObject, ob_size),
			  &name_size) < 0) {
			free (filename);
			break;
		}
		char *name = xmalloc(name_size + 1);
		name[name_size] = '\0';
		if (umovestr(tcp, co_name + offsetof(PyStringObject, ob_sval),
			     name_size, name) < 0) {
			free (filename);
			free(name);
			break;
		}
		call_action (action_data, filename, name, 0, 0);
		free (filename);
		free (name);
		if (umove(tcp, frame + offsetof(PyFrameObject, f_back),
			  &frame) < 0)
			break;
	} while (frame != 0);
	
	return true;
}

static void
subunwind_python2_finish(struct subunwind_context_t *ctx)
{
	struct subunwind_python2_context_t *python2_ctx
		= (struct subunwind_python2_context_t *)ctx;

	free (python2_ctx->expected_modname);
	free (ctx);
}

struct subunwind_unwinder_t subunwind_python2 = {
	.name   = "python2",
	.probe  = subunwind_python2_probe,
	.walk   = subunwind_python2_walk,
	.finish = subunwind_python2_finish,
};
