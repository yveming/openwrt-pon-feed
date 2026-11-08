// SPDX-License-Identifier: GPL-2.0-only
/*
 * iptv - carrier IPTV VLAN demultiplexer.
 *
 * Creates one untagged Ethernet device over a lower device and maps two
 * operator VLANs onto it.  Upstream, unicast frames are tagged with the
 * service VID and IPv4/IPv6 multicast frames with the multicast VID;
 * downstream, frames carrying either VID are stripped and delivered
 * untagged.  Applications see a single interface carrying both the
 * service data and the multicast streams, and the operator VID plan
 * never materialises as separate netdevs.
 *
 * Usage:
 *   ip link add name itv link pon0 type iptv
 *   echo 43 3990 > /sys/class/net/itv/iptv/vids
 *   ip link set itv address 02:00:00:00:00:01
 *
 * Hardware offload: plugins bind iptv_offload_ops to the device (see
 * iptv_offload.h).  Without a plugin the device works purely in
 * software, which keeps the generic layer free of any hardware
 * dependency.
 */

#include <linux/etherdevice.h>
#include <linux/if_arp.h>
#include <linux/if_vlan.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/rtnetlink.h>
#include <net/gro_cells.h>
#include <net/rtnetlink.h>

#include "iptv_offload.h"

/*
 * The operator VIDs are per-device configuration, written through the
 * device's sysfs ("echo <service> <mcast> > .../iptv/vids") by the
 * interface configuration.  An unconfigured device stays inert: its
 * receive path passes everything through and its transmit path drops,
 * so creating it never disturbs the lower device.
 */

static struct rtnl_link_ops iptv_link_ops;

/*
 * v1 carries a single demux device.  The rx_handler receives no private
 * data argument, so the instance is published through this pointer; it
 * is RCU-protected because the receive path walks it.
 */
static struct net_device __rcu *iptv_instance;

struct iptv_dev {
	struct net_device *lower;
	netdevice_tracker dev_tracker;
	unsigned int service_vid;
	unsigned int mcast_vid;
	bool handler_on;
	bool gro_cells_ready;
	struct gro_cells gro_cells;
	const struct iptv_offload_ops *offload;
	void *offload_priv;
};

static bool iptv_vid_valid(unsigned int vid)
{
	return vid >= 1 && vid <= VLAN_VID_MASK;
}

static bool iptv_is_ip_multicast_da(const u8 *addr)
{
	/* The vendor ponvlan rule: IPv4 and IPv6 multicast only; broadcast
	 * (DHCP) stays on the service VLAN. */
	return (addr[0] == 0x01 && addr[1] == 0x00 && addr[2] == 0x5e) ||
	       (addr[0] == 0x33 && addr[1] == 0x33);
}

/* ------------------------------------------------------------------ */
/* Downstream: demux the lower device's receive path                   */
/* ------------------------------------------------------------------ */

static rx_handler_result_t iptv_rx_handler(struct sk_buff **pskb)
{
	struct sk_buff *skb = *pskb;
	struct net_device *dev, *lower = skb->dev;
	struct iptv_dev *iptv;
	unsigned int vid, tci;

	dev = rcu_dereference(iptv_instance);
	if (!dev)
		return RX_HANDLER_PASS;
	iptv = netdev_priv(dev);
	if (iptv->lower != lower)
		return RX_HANDLER_PASS;

	if (skb_vlan_tag_present(skb)) {
		vid = skb_vlan_tag_get_id(skb);
	} else if (eth_type_vlan(skb->protocol) &&
		   skb_mac_header_was_set(skb)) {
		struct vlan_ethhdr *veth = (struct vlan_ethhdr *)skb_mac_header(skb);

		vid = ntohs(veth->h_vlan_TCI) & VLAN_VID_MASK;
	} else {
		return RX_HANDLER_PASS;	/* untagged: not ours */
	}

	if (vid != iptv->service_vid && vid != iptv->mcast_vid)
		return RX_HANDLER_PASS;

	if (skb_vlan_tag_present(skb)) {
		tci = skb_vlan_tag_get(skb);
		__vlan_hwaccel_clear_tag(skb);
	} else {
		skb = skb_vlan_untag(skb);
		if (!skb)
			return RX_HANDLER_CONSUMED;	/* freed by untag */
		tci = skb_vlan_tag_get(skb);
		__vlan_hwaccel_clear_tag(skb);
	}
	*pskb = skb;

	/* eth_type_trans() classified against the lower device's MAC; the
	 * logical device owns its own MAC, so re-check like
	 * vlan_do_receive() does and only upgrade really-ours frames. */
	if (unlikely(skb->pkt_type == PACKET_OTHERHOST) &&
	    ether_addr_equal_64bits(eth_hdr(skb)->h_dest, dev->dev_addr))
		skb->pkt_type = PACKET_HOST;

	skb->priority = (tci & VLAN_PRIO_MASK) >> VLAN_PRIO_SHIFT;
	skb->dev = dev;
	dev_sw_netstats_rx_add(dev, skb->len);
	gro_cells_receive(&iptv->gro_cells, skb);
	return RX_HANDLER_CONSUMED;
}

/* ------------------------------------------------------------------ */
/* Upstream: tag and hand to the lower device                          */
/* ------------------------------------------------------------------ */

static netdev_tx_t iptv_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct iptv_dev *iptv = netdev_priv(dev);
	struct net_device *lower = iptv->lower;
	unsigned int len = skb->len;
	unsigned int vid;

	if (unlikely(!lower || !netif_running(lower) || !netif_carrier_ok(lower) ||
		     skb->len < ETH_HLEN ||
		     !iptv_vid_valid(iptv->service_vid) ||
		     !iptv_vid_valid(iptv->mcast_vid)))
		goto drop;

	if (unlikely(skb_vlan_tag_present(skb) ||
		     eth_type_vlan(((struct ethhdr *)skb->data)->h_proto))) {
		/* The logical service interface is defined as untagged. */
		goto drop;
	}

	vid = iptv_is_ip_multicast_da(skb->data) ? iptv->mcast_vid
						 : iptv->service_vid;

	dev_sw_netstats_tx_add(dev, 1, len);
	/* The tag rides in skb metadata; the lower device inserts it on the
	 * wire (hardware VLAN offload or the core's software fallback). */
	__vlan_hwaccel_put_tag(skb, htons(ETH_P_8021Q), vid);
	skb->dev = lower;
	dev_queue_xmit(skb);

	return NETDEV_TX_OK;

drop:
	dev->stats.tx_dropped++;
	dev_kfree_skb_any(skb);
	return NETDEV_TX_OK;
}

/* ------------------------------------------------------------------ */
/* per-device operator VID configuration (sysfs)                       */
/* ------------------------------------------------------------------ */

static ssize_t iptv_vids_show(struct device *d, struct device_attribute *attr,
			      char *buf)
{
	struct iptv_dev *iptv = netdev_priv(to_net_dev(d));

	return sysfs_emit(buf, "%u %u\n", iptv->service_vid, iptv->mcast_vid);
}

static ssize_t iptv_vids_store(struct device *d, struct device_attribute *attr,
			       const char *buf, size_t count)
{
	struct iptv_dev *iptv = netdev_priv(to_net_dev(d));
	unsigned int service, mcast;
	int ret;

	if (sscanf(buf, "%u %u", &service, &mcast) != 2)
		return -EINVAL;

	rtnl_lock();

	if (iptv_vid_valid(service) && iptv_vid_valid(mcast)) {
		if (!iptv->handler_on) {
			ret = netdev_rx_handler_register(iptv->lower,
							 iptv_rx_handler, NULL);
			if (ret) {
				rtnl_unlock();
				return ret;
			}
			iptv->handler_on = true;
		}
		iptv->service_vid = service;
		iptv->mcast_vid = mcast;
	} else if (service == 0 && mcast == 0) {
		/* explicit disarm: the lower's receive path is left
		 * untouched again */
		if (iptv->handler_on) {
			netdev_rx_handler_unregister(iptv->lower);
			iptv->handler_on = false;
		}
		iptv->service_vid = 0;
		iptv->mcast_vid = 0;
	} else {
		rtnl_unlock();
		return -EINVAL;
	}

	rtnl_unlock();
	return count;
}
static DEVICE_ATTR(vids, 0644, iptv_vids_show, iptv_vids_store);

static struct attribute *iptv_sysfs_attrs[] = {
	&dev_attr_vids.attr,
	NULL,
};
static const struct attribute_group iptv_sysfs_group = {
	.attrs = iptv_sysfs_attrs,
};

/* ------------------------------------------------------------------ */
/* netdev ops: mirror a standard VLAN upper                            */
/* ------------------------------------------------------------------ */

static void
iptv_set_rx_mode(struct net_device *dev)
{
	struct iptv_dev *iptv = netdev_priv(dev);
	struct net_device *lower = iptv->lower;

	if (!lower)
		return;

	dev_mc_sync(lower, dev);
	dev_uc_sync(lower, dev);
}

static void
iptv_change_rx_flags(struct net_device *dev, int change)
{
	struct iptv_dev *iptv = netdev_priv(dev);
	struct net_device *lower = iptv->lower;

	if (!lower)
		return;

	if (change & IFF_ALLMULTI)
		dev_set_allmulti(lower, dev->flags & IFF_ALLMULTI ? 1 : -1);
	if (change & IFF_PROMISC)
		dev_set_promiscuity(lower, dev->flags & IFF_PROMISC ? 1 : -1);
}

static int
iptv_set_mac_address(struct net_device *dev, void *p)
{
	struct iptv_dev *iptv = netdev_priv(dev);
	struct net_device *lower = iptv->lower;
	struct sockaddr *addr = p;
	int ret;

	if (!is_valid_ether_addr(addr->sa_data))
		return -EADDRNOTAVAIL;

	if (lower && (dev->flags & IFF_UP)) {
		if (!ether_addr_equal(addr->sa_data, lower->dev_addr)) {
			ret = dev_uc_add(lower, addr->sa_data);
			if (ret < 0)
				return ret;
		}
		if (!ether_addr_equal(dev->dev_addr, lower->dev_addr))
			dev_uc_del(lower, dev->dev_addr);
	}

	eth_hw_addr_set(dev, addr->sa_data);
	return 0;
}

static netdev_features_t
iptv_fix_features(struct net_device *dev, netdev_features_t features)
{
	struct iptv_dev *iptv = netdev_priv(dev);
	struct net_device *lower = iptv->lower;
	netdev_features_t old_features = features;
	netdev_features_t lower_features;

	if (!lower)
		return features;

	lower_features = netdev_intersect_features(
		(lower->vlan_features | NETIF_F_RXCSUM), lower->features);
	if (lower_features & (NETIF_F_IP_CSUM | NETIF_F_IPV6_CSUM))
		lower_features |= NETIF_F_HW_CSUM;

	features = netdev_intersect_features(features, lower_features);
	features |= old_features & (NETIF_F_SOFT_FEATURES |
				    NETIF_F_GSO_SOFTWARE_ALL);
	return features;
}

static int
iptv_open(struct net_device *dev)
{
	struct iptv_dev *iptv = netdev_priv(dev);
	struct net_device *lower = iptv->lower;
	int ret;

	if (lower && !ether_addr_equal(dev->dev_addr, lower->dev_addr)) {
		ret = dev_uc_add(lower, dev->dev_addr);
		if (ret < 0)
			return ret;
	}

	if (lower && netif_carrier_ok(lower)) {
		netif_carrier_on(dev);
		netif_tx_wake_all_queues(dev);
	}

	return 0;
}

static int
iptv_stop(struct net_device *dev)
{
	struct iptv_dev *iptv = netdev_priv(dev);
	struct net_device *lower = iptv->lower;

	if (lower) {
		dev_mc_unsync(lower, dev);
		dev_uc_unsync(lower, dev);
		if (!ether_addr_equal(dev->dev_addr, lower->dev_addr))
			dev_uc_del(lower, dev->dev_addr);
	}

	netif_carrier_off(dev);
	netif_tx_stop_all_queues(dev);
	return 0;
}

static int
iptv_get_iflink(const struct net_device *dev)
{
	const struct iptv_dev *iptv = netdev_priv(dev);

	return iptv->lower ? iptv->lower->ifindex : dev->ifindex;
}

static int
iptv_setup_tc(struct net_device *dev, enum tc_setup_type type, void *type_data)
{
	/* v1: no flow-table offload.  The plugin layer registers
	 * iptv_offload_ops and the generic TC plumbing grows there. */
	return -EOPNOTSUPP;
}

static const struct net_device_ops iptv_netdev_ops = {
	.ndo_open		= iptv_open,
	.ndo_stop		= iptv_stop,
	.ndo_start_xmit		= iptv_xmit,
	.ndo_set_mac_address	= iptv_set_mac_address,
	.ndo_set_rx_mode	= iptv_set_rx_mode,
	.ndo_change_rx_flags	= iptv_change_rx_flags,
	.ndo_fix_features	= iptv_fix_features,
	.ndo_get_stats64	= dev_get_tstats64,
	.ndo_get_iflink		= iptv_get_iflink,
	.ndo_validate_addr	= eth_validate_addr,
	.ndo_setup_tc		= iptv_setup_tc,
};

static void iptv_priv_free(struct net_device *dev);

static void iptv_setup(struct net_device *dev)
{
	ether_setup(dev);
	dev->netdev_ops = &iptv_netdev_ops;
	/* Same software feature set as a standard VLAN upper: ndo_fix_features
	 * intersects with what the lower device supports, so forwarded GSO
	 * and TCP checksum offloads survive the demux. */
	dev->hw_features = NETIF_F_HW_CSUM | NETIF_F_SG | NETIF_F_FRAGLIST |
			   NETIF_F_GSO_SOFTWARE_ALL | NETIF_F_GSO_ENCAP_ALL |
			   NETIF_F_HIGHDMA | NETIF_F_RXCSUM | NETIF_F_GRO;
	dev->features = dev->hw_features;
	/* v1: NETIF_F_HW_TC deliberately off - the offload plugin enables
	 * hardware flow offload once its ops are registered. */
	dev->lltx = true;
	dev->pcpu_stat_type = NETDEV_PCPU_STAT_TSTATS;
	dev->needs_free_netdev = true;
	dev->priv_destructor = iptv_priv_free;
}

/* ------------------------------------------------------------------ */
/* create / destroy                                                    */
/* ------------------------------------------------------------------ */

/* Runs once, when the core frees the device - on both the explicit
 * delete path and the lower-device-unregister path. */
static void iptv_priv_free(struct net_device *dev)
{
	struct iptv_dev *iptv = netdev_priv(dev);

	if (iptv->gro_cells_ready)
		gro_cells_destroy(&iptv->gro_cells);
	if (iptv->lower)
		netdev_put(iptv->lower, &iptv->dev_tracker);
}

static void iptv_detach_lower(struct iptv_dev *iptv)
{
	struct net_device *lower = iptv->lower;

	rcu_assign_pointer(iptv_instance, NULL);

	if (lower && iptv->handler_on) {
		netdev_rx_handler_unregister(lower);
		iptv->handler_on = false;
	}

	/* The receive hook runs under rcu_read_lock(); wait for an
	 * in-flight frame before the GRO state that owns an skb handed to
	 * this device goes away. */
	synchronize_net();
}

static int iptv_newlink(struct net_device *dev,
			struct rtnl_newlink_params *params,
			struct netlink_ext_ack *extack)
{
	struct iptv_dev *iptv = netdev_priv(dev);
	struct net *net = rtnl_newlink_link_net(params);
	struct nlattr **tb = params->tb;
	struct net_device *lower;
	int ret;

	if (!tb[IFLA_LINK]) {
		NL_SET_ERR_MSG(extack,
			       "iptv devices require a lower device (ip link add ... link LOWER type iptv)");
		return -EINVAL;
	}
	lower = __dev_get_by_index(net, nla_get_u32(tb[IFLA_LINK]));
	if (!lower) {
		NL_SET_ERR_MSG(extack, "lower device not found");
		return -ENODEV;
	}
	if (lower->rtnl_link_ops == &iptv_link_ops) {
		NL_SET_ERR_MSG(extack, "nesting iptv devices is not supported");
		return -EINVAL;
	}

	if (rcu_access_pointer(iptv_instance)) {
		NL_SET_ERR_MSG(extack, "only one iptv device is supported");
		return -EBUSY;
	}

	dev->needed_headroom = lower->needed_headroom + VLAN_HLEN;
	dev->hard_header_len = lower->hard_header_len;
	dev->vlan_features = lower->vlan_features &
			     ~(NETIF_F_FCOE_CRC | NETIF_F_FSO);
	eth_hw_addr_set(dev, lower->dev_addr);
	netif_inherit_tso_max(dev, lower);

	ret = gro_cells_init(&iptv->gro_cells, dev);
	if (ret)
		return ret;
	iptv->gro_cells_ready = true;

	iptv->lower = lower;
	netdev_hold(lower, &iptv->dev_tracker, GFP_KERNEL);

	ret = register_netdevice(dev);
	if (ret)
		goto err_ref;

	ret = sysfs_create_group(&dev->dev.kobj, &iptv_sysfs_group);
	if (ret)
		goto err_unregister;

	ret = netdev_upper_dev_link(lower, dev, extack);
	if (ret)
		goto err_sysfs;

	rcu_assign_pointer(iptv_instance, dev);
	if (netif_carrier_ok(lower)) {
		netif_carrier_on(dev);
		netif_tx_wake_all_queues(dev);
	}

	return 0;

err_sysfs:
	sysfs_remove_group(&dev->dev.kobj, &iptv_sysfs_group);
err_unregister:
	unregister_netdevice(dev);
	return ret;

err_ref:
	netdev_put(lower, &iptv->dev_tracker);
	iptv->lower = NULL;
	return ret;
}

static void iptv_dellink(struct net_device *dev, struct list_head *head)
{
	struct iptv_dev *iptv = netdev_priv(dev);

	if (iptv->lower)
		netdev_upper_dev_unlink(iptv->lower, dev);
	iptv_detach_lower(iptv);
	sysfs_remove_group(&dev->dev.kobj, &iptv_sysfs_group);
	unregister_netdevice_queue(dev, head);
}

static struct rtnl_link_ops iptv_link_ops = {
	.kind		= "iptv",
	.priv_size	= sizeof(struct iptv_dev),
	.setup		= iptv_setup,
	.newlink	= iptv_newlink,
	.dellink	= iptv_dellink,
};

/* ------------------------------------------------------------------ */
/* lower-device events: carrier sync and auto-teardown                 */
/* ------------------------------------------------------------------ */

static int iptv_netdev_event(struct notifier_block *nb, unsigned long event,
			     void *ptr)
{
	struct net_device *lower = netdev_notifier_info_to_dev(ptr);
	struct net_device *dev;
	struct iptv_dev *iptv;

	rcu_read_lock();
	dev = rcu_dereference(iptv_instance);
	iptv = netdev_priv(dev);
	if (!dev || iptv->lower != lower) {
		rcu_read_unlock();
		return NOTIFY_DONE;
	}
	rcu_read_unlock();

	switch (event) {
	case NETDEV_CHANGE:
		if (netif_carrier_ok(lower)) {
			netif_carrier_on(dev);
			netif_tx_wake_all_queues(dev);
		} else {
			netif_carrier_off(dev);
			netif_tx_stop_all_queues(dev);
		}
		break;
	case NETDEV_UNREGISTER:
		/* The lower device is going away; the demux device cannot
		 * survive it.  Runs with the rtnl held, and mirrors what
		 * the vlan layer does for its uppers. */
		if (lower->reg_state == NETREG_UNREGISTERING) {
			netdev_upper_dev_unlink(lower, dev);
			iptv_detach_lower(netdev_priv(dev));
			sysfs_remove_group(&dev->dev.kobj, &iptv_sysfs_group);
			unregister_netdevice(dev);
		}
		break;
	}

	return NOTIFY_DONE;
}

static struct notifier_block iptv_netdev_notifier = {
	.notifier_call = iptv_netdev_event,
};

/* ------------------------------------------------------------------ */
/* offload seam                                                        */
/* ------------------------------------------------------------------ */

int iptv_offload_register(struct net_device *dev,
			  const struct iptv_offload_ops *ops, void *priv)
{
	struct iptv_dev *iptv;

	if (dev->rtnl_link_ops != &iptv_link_ops)
		return -EINVAL;

	iptv = netdev_priv(dev);
	iptv->offload = ops;
	iptv->offload_priv = priv;
	return 0;
}
EXPORT_SYMBOL_GPL(iptv_offload_register);

void iptv_offload_unregister(struct net_device *dev)
{
	struct iptv_dev *iptv = netdev_priv(dev);

	iptv->offload = NULL;
	iptv->offload_priv = NULL;
}
EXPORT_SYMBOL_GPL(iptv_offload_unregister);

/* ------------------------------------------------------------------ */

static int __init iptv_init(void)
{
	int ret;

	ret = register_netdevice_notifier(&iptv_netdev_notifier);
	if (ret)
		return ret;

	ret = rtnl_link_register(&iptv_link_ops);
	if (ret)
		unregister_netdevice_notifier(&iptv_netdev_notifier);
	return ret;
}

static void __exit iptv_exit(void)
{
	rtnl_link_unregister(&iptv_link_ops);
	unregister_netdevice_notifier(&iptv_netdev_notifier);
}

module_init(iptv_init);
module_exit(iptv_exit);

MODULE_DESCRIPTION("Carrier IPTV VLAN demultiplexer");
MODULE_LICENSE("GPL");
