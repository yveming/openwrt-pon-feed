# SPDX-License-Identifier: Apache-2.0
#
# Thin package for the iptv-scripts source kept in yveming/openwrt-iptv.

include $(TOPDIR)/rules.mk

PKG_NAME:=iptv-scripts
PKG_VERSION:=0.1.0
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/yveming/openwrt-iptv.git
PKG_SOURCE_VERSION:=668506be03cc1586ddd525ebfad65b169252819d
PKG_SOURCE_DATE:=2026-11-08
PKG_SOURCE_SUBDIR:=openwrt-iptv-2026.11.08~668506b
PKG_SOURCE:=openwrt-iptv-2026.11.08~668506b.tar.zst
PKG_MIRROR_HASH:=a4ef476a998e15d1a753a7a34e5f051b3bbcf5158b9fb453b72debdde3d8d2a9

PKG_MAINTAINER:=ming
PKG_LICENSE:=Apache-2.0

include $(INCLUDE_DIR)/package.mk

define Package/iptv-scripts
  SECTION:=net
  CATEGORY:=Network
  TITLE:=IPTV device setup scripts
  DEPENDS:=+kmod-iptv
  PKGARCH:=all
endef

define Package/iptv-scripts/description
 UCI configuration, boot script and apply helper that create the itv
 device from kmod-iptv. Configure addressing and bridging on itv with
 the standard Network pages.
endef

define Package/iptv-scripts/conffiles
/etc/config/iptv
endef

define Build/Prepare
	rm -rf $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)
	$(STAGING_DIR_HOST)/bin/zstd -dc $(DL_DIR)/$(PKG_SOURCE) | \
		$(TAR) -x -C $(PKG_BUILD_DIR) --strip-components=3 \
			$(PKG_SOURCE_SUBDIR)/net/iptv-scripts
	$(Build/Patch)
endef

define Build/Compile
endef

define Package/iptv-scripts/install
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/files/etc/config/iptv $(1)/etc/config/iptv

	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/etc/init.d/iptv $(1)/etc/init.d/iptv

	$(INSTALL_DIR) $(1)/usr/libexec
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/usr/libexec/iptv-apply $(1)/usr/libexec/iptv-apply

	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/etc/uci-defaults/30-iptv-stb-ignore \
		$(1)/etc/uci-defaults/30-iptv-stb-ignore
endef

$(eval $(call BuildPackage,iptv-scripts))
