/*
 * Copyright (c) 2022 The strace developers.
 * All rights reserved.
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef STRACE_NETLINK_GENERIC_H
# define STRACE_NETLINK_GENERIC_H

typedef void (*netlink_generic_decoder_t)(struct tcb *const tcp,
					  uint8_t cmd, uint8_t version,
					  const kernel_ulong_t addr,
					  const unsigned int len);
struct netlink_generic_decoder {
	uint16_t id;
	const char *type;
	const struct xlat *cmd_xlat;
	const char *cmd_dflt;
	const netlink_generic_decoder_t decode_payload;
};

const struct netlink_generic_decoder * lookup_netlink_generic_decoder(const uint16_t type_id);;

#define NETLINK_GENERIC_DECODER(name, cmd_dflt) \
	{ 0, #name, genl_##name##_cmds, cmd_dflt, decode_netlink_generic_##name, }

#define DECLARE_NETLINK_GENERIC_DECODER(name)	 \
extern void \
decode_netlink_generic_##name(struct tcb *const tcp, \
			      uint8_t cmd, uint8_t version, \
			      const kernel_ulong_t addr, \
			      const unsigned int len)

#define NETLINK_GENERIC_NLCTRL_DECODER NETLINK_GENERIC_DECODER(nlctrl, "CTRL_CMD_???")
DECLARE_NETLINK_GENERIC_DECODER(nlctrl);

#endif	/* !STRACE_NETLINK_GENERIC_H */
