# SPDX-License-Identifier: GPL-2.0-only
#
# Thin package for the kmod-iptv source kept in yveming/openwrt-iptv.

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=iptv
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/yveming/openwrt-iptv.git
PKG_SOURCE_VERSION:=668506be03cc1586ddd525ebfad65b169252819d
PKG_SOURCE_DATE:=2026-11-08
PKG_SOURCE_SUBDIR:=openwrt-iptv-2026.11.08~668506b
PKG_SOURCE:=openwrt-iptv-2026.11.08~668506b.tar.zst
PKG_MIRROR_HASH:=a4ef476a998e15d1a753a7a34e5f051b3bbcf5158b9fb453b72debdde3d8d2a9

include $(INCLUDE_DIR)/package.mk

define KernelPackage/iptv
  SUBMENU:=Network Devices
  TITLE:=IPTV interface support
  FILES:=$(PKG_BUILD_DIR)/src/iptv.ko
  AUTOLOAD:=$(call AutoLoad,46,iptv)
  KCONFIG:=
endef

define KernelPackage/iptv/description
  A single untagged Ethernet device carrying one unicast VLAN and one
  multicast VLAN over the uplink.
endef

define Build/Prepare
	rm -rf $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)
	$(STAGING_DIR_HOST)/bin/zstd -dc $(DL_DIR)/$(PKG_SOURCE) | \
		$(TAR) -x -C $(PKG_BUILD_DIR) --strip-components=3 \
			$(PKG_SOURCE_SUBDIR)/kernel/iptv
	$(Build/Patch)
endef

define Build/Compile
	$(KERNEL_MAKE) M="$(PKG_BUILD_DIR)/src" modules
endef

$(eval $(call KernelPackage,iptv))
