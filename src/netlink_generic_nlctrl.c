/*
 * Copyright (c) 2022 The strace developers.
 * All rights reserved.
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "defs.h"

#include "netlink_generic.h"
#include "nlattr.h"

#include <linux/genetlink.h>

#include "xlat/genl_nlctrl_ctrl_attrs.h"
#include "xlat/genl_nlctrl_ctrl_attr_op_attrs.h"
#include "xlat/genl_nlctrl_ctrl_ops_flags.h"


typedef void (*netlink_generic_decoder_nlctrl_t)(struct tcb *const tcp,
						 uint8_t cmd, uint8_t verison,
						 const kernel_ulong_t addr,
						 const unsigned int len);

struct netlink_generic_nlctrl_decoder {
	uint8_t cmd;
	uint8_t version;
	netlink_generic_decoder_nlctrl_t decode;
};

static bool
decode_nla_u32_with_cmd_decoder(struct tcb *const tcp,
				const kernel_ulong_t addr,
				const unsigned int len,
				const void *const opaque_data)
{
	const struct netlink_generic_decoder *cmd_decoder = opaque_data;
	uint32_t num;

	if (len < sizeof(num))
		return false;
	if (!umove_or_printaddr(tcp, addr, &num))
		printxval_u(cmd_decoder->cmd_xlat, num, cmd_decoder->cmd_dflt);
	return true;
}

static bool
decode_nla_genl_ops_flags(struct tcb *const tcp,
			  const kernel_ulong_t addr,
			  const unsigned int len,
			  const void *const opaque_data)
{
	uint32_t num;

	if (len < sizeof(num))
		return false;
	if (!umove_or_printaddr(tcp, addr, &num))
		printflags(genl_nlctrl_ctrl_ops_flags, num, "GENL_???");
	return true;
}

static const nla_decoder_t ctrl_attr_op_nla_decoders[] = {
	[CTRL_ATTR_OP_ID] = decode_nla_u32_with_cmd_decoder,
	[CTRL_ATTR_OP_FLAGS] = decode_nla_genl_ops_flags,
};

static bool
decode_ctrl_attr_op(struct tcb *const tcp,
		    const kernel_ulong_t addr,
		    const unsigned int len,
		    const void *const opaque_data)
{
	uint16_t family = *(uint16_t *)opaque_data;
	const struct netlink_generic_decoder *cmd_decoder =
		lookup_netlink_generic_decoder(family);

	decode_nlattr(tcp, addr, len,
		      genl_nlctrl_ctrl_attr_op_attrs, "CTRL_ATTR_OP_???",
		      ARRSZ_PAIR(ctrl_attr_op_nla_decoders),
		      cmd_decoder);
	return true;
}

static bool
decode_ctrl_attr_ops(struct tcb *const tcp,
		     const kernel_ulong_t addr,
		     const unsigned int len,
		     const void *const opaque_data)
{
	nla_decoder_t decoder = &decode_ctrl_attr_op;
	decode_nlattr(tcp, addr, len,
		      NULL, 0,
		      &decoder, 0, opaque_data);
	return true;
}

static bool
decode_nla_u16_with_capturing_family(struct tcb *const tcp,
				     const kernel_ulong_t addr,
				     const unsigned int len,
				     const void *const opaque_data)
{
	uint16_t num;

	if (len < sizeof(num))
		return false;
	if (!umove_or_printaddr(tcp, addr, &num)) {
		tprintf("%" PRIu16, num);
		*((uint16_t *)opaque_data) = num;
	}
	return true;
}

static const nla_decoder_t ctrl_attr_nla_decoders[] = {
	[CTRL_ATTR_FAMILY_ID] = decode_nla_u16_with_capturing_family,
	[CTRL_ATTR_FAMILY_NAME] = decode_nla_str,
	[CTRL_ATTR_VERSION] = decode_nla_u32,
	[CTRL_ATTR_HDRSIZE] = decode_nla_u32,
	[CTRL_ATTR_MAXATTR] = decode_nla_u32,
	[CTRL_ATTR_OPS] = decode_ctrl_attr_ops,

	/* NOT IMPLEMENTED YET */
	[CTRL_ATTR_MCAST_GROUPS] = NULL,
	[CTRL_ATTR_POLICY] = NULL,
	[CTRL_ATTR_OP_POLICY] = NULL,
	[CTRL_ATTR_OP] = NULL,
};

static void
decode_netlink_generic_nlctrl_newfamily(struct tcb *const tcp,
					uint8_t cmd, uint8_t verison,
					const kernel_ulong_t addr,
					const unsigned int len)
{
	uint16_t family = 0;
	decode_nlattr(tcp, addr, len, genl_nlctrl_ctrl_attrs,
		      "CTRL_ATTR_???", ARRSZ_PAIR(ctrl_attr_nla_decoders),
		      &family);
}

static const struct netlink_generic_nlctrl_decoder generic_nlctrl_decoders[] = {
	{ CTRL_CMD_NEWFAMILY, 2,
	  decode_netlink_generic_nlctrl_newfamily },
};

static const struct netlink_generic_nlctrl_decoder *
find_netlink_generic_nlctrl_decoder(uint8_t cmd, uint8_t version)
{
	for (size_t i = 0; i < ARRAY_SIZE(generic_nlctrl_decoders); i++) {
		if (generic_nlctrl_decoders[i].cmd == cmd
		    && generic_nlctrl_decoders[i].version == version)
			return generic_nlctrl_decoders + i;
	}
	return NULL;
}

void
decode_netlink_generic_nlctrl(struct tcb *const tcp,
			      uint8_t cmd, uint8_t version,
			      const kernel_ulong_t addr,
			      const unsigned int len)
{
	const struct netlink_generic_nlctrl_decoder *decoder
		= find_netlink_generic_nlctrl_decoder(cmd, version);
	if (decoder)
		decoder->decode (tcp,
				 cmd, version,
				 addr, len);
	else {
		tprint_array_next();
		printstr_ex(tcp, addr, len, QUOTE_FORCE_HEX);
	}
}
