/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _IPTV_OFFLOAD_H_
#define _IPTV_OFFLOAD_H_

#include <linux/netdevice.h>

/**
 * Hardware offload plugin interface.
 *
 * A plugin binds per iptv device and moves classified flows into its
 * engine.  The generic layer owns the demux view (one untagged device
 * over a lower device carrying two operator VIDs); the plugin translates
 * that into the hardware's matching view.  The flow-table integration
 * grows here together with the generic TC plumbing.
 */
struct iptv_offload_ops {
	void (*probe)(struct net_device *dev, void *priv);
};

int iptv_offload_register(struct net_device *dev,
			  const struct iptv_offload_ops *ops, void *priv);
void iptv_offload_unregister(struct net_device *dev);
#endif /* _IPTV_OFFLOAD_H_ */
