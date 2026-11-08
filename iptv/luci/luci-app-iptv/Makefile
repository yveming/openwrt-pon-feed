# SPDX-License-Identifier: Apache-2.0
#
# Thin package for luci-app-iptv kept in yveming/openwrt-iptv.

include $(TOPDIR)/rules.mk

LUCI_TITLE:=LuCI IPTV device configuration
LUCI_DEPENDS:=+luci-base +kmod-iptv
LUCI_PKGARCH:=all

PKG_VERSION:=0.1.0
PKG_RELEASE:=1
PKG_PO_VERSION:=0.1.0
PKG_LICENSE:=Apache-2.0

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/yveming/openwrt-iptv.git
PKG_SOURCE_VERSION:=ed62fbefa6cb23602dbf3a6c15cab6e749784e60
PKG_SOURCE_DATE:=2026-11-08
PKG_SOURCE_SUBDIR:=openwrt-iptv-2026.11.08~ed62fbe
PKG_SOURCE:=openwrt-iptv-2026.11.08~ed62fbe.tar.zst
PKG_MIRROR_HASH:=a0e3284506d085fb4e38cac6669ec859759d14701d9cbb4dca02ea31186fb879

define Build/Prepare/luci-app-iptv
	$(STAGING_DIR_HOST)/bin/zstd -dc $(DL_DIR)/$(PKG_SOURCE) | \
		$(TAR) -x -C $(PKG_BUILD_DIR) --strip-components=3 \
			$(PKG_SOURCE_SUBDIR)/luci/luci-app-iptv
endef

include $(TOPDIR)/feeds/luci/luci.mk

# Thin package: po/ is extracted from the upstream tarball in Build/Prepare.
# luci.mk only scans ${CURDIR}/po at metadata time, so define zh_Hans
# explicitly and compile it from the build dir.
$(eval $(call LuciTranslation,zh-cn,zh_Hans))

define Package/luci-i18n-iptv-zh-cn/install
	$(INSTALL_DIR) $(1)/etc/uci-defaults
	echo "uci set luci.languages.zh_Hans='zh_Hans'; uci commit luci" \
		> $(1)/etc/uci-defaults/luci-i18n-iptv-zh-cn
	$(INSTALL_DIR) $(1)$(LUCI_LIBRARYDIR)/i18n
	$(foreach po,$(wildcard $(PKG_BUILD_DIR)/po/zh_Hans/*.po), \
		po2lmo $(po) \
			$(1)$(LUCI_LIBRARYDIR)/i18n/$(basename $(notdir $(po))).zh-cn.lmo;)
endef

$(eval $(call BuildPackage,luci-i18n-iptv-zh-cn))

# call BuildPackage: feed scanner marker; luci.mk generates the package rules.
