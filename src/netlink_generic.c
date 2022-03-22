/*
 * Copyright (c) 2022 The strace developers.
 * All rights reserved.
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "defs.h"

#include "netlink.h"
#include "netlink_generic.h"

#include <linux/genetlink.h>
#include "xlat/genl_nlctrl_cmds.h"

static bool generic_decoders_id_assigned;
static struct netlink_generic_decoder generic_decoders[] = {
	NETLINK_GENERIC_NLCTRL_DECODER,
};

static const struct netlink_generic_decoder fallback_decoder = {
	0, "???", NULL, "???", NULL,
};

static void
assign_ids_to_generic_decoders(struct tcb *const tcp)
{
	const struct xlat *xlat = genl_families_xlat(tcp);
	for (size_t i = 0; i < ARRAY_SIZE(generic_decoders); i++)
		generic_decoders[i].id = xrlookup(xlat, generic_decoders[i].type, 0);
}

const struct netlink_generic_decoder *
lookup_netlink_generic_decoder(const uint16_t type_id)
{
	for (size_t i = 0; i < ARRAY_SIZE(generic_decoders); i++) {
		if (type_id == generic_decoders[i].id)
			return generic_decoders + i;
	}
	return &fallback_decoder;
}

bool
decode_netlink_generic(struct tcb *const tcp,
		       const struct nlmsghdr *const nlmsghdr,
		       const kernel_ulong_t addr,
		       const unsigned int len)
{
	if (nlmsghdr->nlmsg_type == NLMSG_DONE)
		return false;

	if (!tcp->last_genl_family)
		return false;

	struct genlmsghdr msghdr;
	if (len < sizeof(msghdr))
		printstr_ex(tcp, addr, len, QUOTE_FORCE_HEX);
	else if (!umove_or_printaddr(tcp, addr, &msghdr)) {
		if (!generic_decoders_id_assigned) {
			assign_ids_to_generic_decoders(tcp);
			generic_decoders_id_assigned = true;
		}

		const struct netlink_generic_decoder *decoder
			= lookup_netlink_generic_decoder(tcp->last_genl_family);
		tprint_struct_begin();
		PRINT_FIELD_XVAL(msghdr, cmd, decoder->cmd_xlat, decoder->cmd_dflt);
		tprint_struct_next();
		PRINT_FIELD_U(msghdr, version);
		tprint_struct_next();
		PRINT_FIELD_X(msghdr, reserved);
		tprint_struct_end();

		const size_t offset = NLMSG_ALIGN(sizeof(msghdr));
		if (len > offset) {
			if (decoder->decode_payload)
				decoder->decode_payload (tcp, msghdr.cmd, msghdr.version,
							 addr + offset, len - offset);
			else {
				tprint_array_next();
				printstr_ex(tcp, addr + offset,
					    len - offset, QUOTE_FORCE_HEX);
			}
		}
	}

	return true;
}
