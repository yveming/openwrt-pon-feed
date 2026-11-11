# SPDX-License-Identifier: GPL-2.0-only
#
# Thin package for the kmod-iptv source kept in yveming/openwrt-iptv.

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=iptv
PKG_RELEASE:=2
PKG_LICENSE:=GPL-2.0-only AND Apache-2.0
PKG_MAINTAINER:=ming

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/yveming/openwrt-iptv.git
PKG_SOURCE_VERSION:=3f128eed40b508340b6f7a77be2752b023d04c05
PKG_SOURCE_DATE:=2026-11-11
PKG_SOURCE_SUBDIR:=openwrt-iptv-2026.11.11~3f128ee
PKG_SOURCE:=openwrt-iptv-2026.11.11~3f128ee.tar.zst
PKG_MIRROR_HASH:=d697e35e61e2695cf4c2285e2f40670b19ac632cf2602a6fa373a28d00003893

# Pin the build dir before package.mk so the stamp paths do not pick up the
# PKG_VERSION which download.mk derives from PKG_SOURCE_DATE/VERSION. Clear
# PKG_VERSION after package.mk to keep the kmod version tied to the kernel.
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

PKG_VERSION:=

define KernelPackage/iptv
  SUBMENU:=Network Devices
  TITLE:=IPTV interface support
  FILES:=$(PKG_BUILD_DIR)/src/iptv.ko
  AUTOLOAD:=$(call AutoLoad,46,iptv)
  KCONFIG:=
endef

define KernelPackage/iptv/description
  A single untagged Ethernet device carrying one unicast VLAN and one
  multicast VLAN over the uplink. The package also installs the UCI
  configuration, boot script and apply logic that create the device;
  neither part is useful without the other.
endef

define KernelPackage/iptv/conffiles
/etc/config/iptv
endef

define KernelPackage/iptv/install
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/files/etc/config/iptv $(1)/etc/config/iptv

	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/etc/init.d/iptv $(1)/etc/init.d/iptv

	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/etc/uci-defaults/30-iptv-stb-ignore \
		$(1)/etc/uci-defaults/30-iptv-stb-ignore
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
