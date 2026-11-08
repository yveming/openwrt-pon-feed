# SPDX-License-Identifier: Apache-2.0
#
# Thin package for luci-app-iptv kept in yveming/openwrt-iptv.

include $(TOPDIR)/rules.mk

LUCI_TITLE:=LuCI IPTV device configuration
LUCI_DEPENDS:=+luci-base +iptv-scripts
LUCI_PKGARCH:=all

PKG_VERSION:=0.1.0
PKG_RELEASE:=1
PKG_PO_VERSION:=0.1.0
PKG_LICENSE:=Apache-2.0

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/yveming/openwrt-iptv.git
PKG_SOURCE_VERSION:=668506be03cc1586ddd525ebfad65b169252819d
PKG_SOURCE_DATE:=2026-11-08
PKG_SOURCE_SUBDIR:=openwrt-iptv-2026.11.08~668506b
PKG_SOURCE:=openwrt-iptv-2026.11.08~668506b.tar.zst
PKG_MIRROR_HASH:=a4ef476a998e15d1a753a7a34e5f051b3bbcf5158b9fb453b72debdde3d8d2a9

define Build/Prepare/luci-app-iptv
	$(STAGING_DIR_HOST)/bin/zstd -dc $(DL_DIR)/$(PKG_SOURCE) | \
		$(TAR) -x -C $(PKG_BUILD_DIR) --strip-components=3 \
			$(PKG_SOURCE_SUBDIR)/luci/luci-app-iptv
endef

include $(TOPDIR)/feeds/luci/luci.mk

# call BuildPackage: feed scanner marker; luci.mk generates the package rules.
