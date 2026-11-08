# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=iptv
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define KernelPackage/iptv
  SUBMENU:=Network Devices
  TITLE:=IPTV interface support
  FILES:=$(PKG_BUILD_DIR)/iptv.ko
  AUTOLOAD:=$(call AutoLoad,46,iptv)
  KCONFIG:=
endef

define KernelPackage/iptv/description
  A single untagged Ethernet device carrying one unicast VLAN and one
  multicast VLAN over the uplink.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) $(CURDIR)/src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(KERNEL_MAKE) M="$(PKG_BUILD_DIR)" modules
endef

$(eval $(call KernelPackage,iptv))
