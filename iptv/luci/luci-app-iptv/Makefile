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
PKG_SOURCE_VERSION:=3f128eed40b508340b6f7a77be2752b023d04c05
PKG_SOURCE_DATE:=2026-11-11
PKG_SOURCE_SUBDIR:=openwrt-iptv-2026.11.11~3f128ee
PKG_SOURCE:=openwrt-iptv-2026.11.11~3f128ee.tar.zst
PKG_MIRROR_HASH:=d697e35e61e2695cf4c2285e2f40670b19ac632cf2602a6fa373a28d00003893

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
	# po2lmo input only exists after Build/Prepare, so glob at install time
	# instead of expanding $(wildcard ...) while the Makefile is parsed.
	for po in $(PKG_BUILD_DIR)/po/zh_Hans/*.po; do \
		[ -e "$$$$po" ] || continue; \
		lmo="$$$${po##*/}"; \
		po2lmo "$$$$po" \
			"$(1)$(LUCI_LIBRARYDIR)/i18n/$$$${lmo%.po}.zh-cn.lmo"; \
	done
endef

$(eval $(call BuildPackage,luci-i18n-iptv-zh-cn))

# call BuildPackage: feed scanner marker; luci.mk generates the package rules.
