# SPDX-License-Identifier: Apache-2.0
#
# Thin package for luci-app-voice kept in yveming/airoha-voice.

include $(TOPDIR)/rules.mk

LUCI_TITLE:=LuCI Voice / SIP configuration
LUCI_DEPENDS:=+luci-base +asterisk +asterisk-chan-an7581 \
	+asterisk-pjsip +asterisk-res-pjproject +ip-full
LUCI_PKGARCH:=all
PKG_VERSION:=0.2.0
PKG_RELEASE:=10
PKG_PO_VERSION:=0.2.0
PKG_LICENSE:=Apache-2.0

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/yveming/airoha-voice.git
PKG_SOURCE_VERSION:=376eccb99ae96d842f8e85fd6e5d1c267afe5932
PKG_SOURCE_DATE:=2026-11-11
PKG_SOURCE_SUBDIR:=airoha-voice-2026-11-11~376eccb
PKG_SOURCE:=airoha-voice-2026-11-11~376eccb.tar.zst
PKG_MIRROR_HASH:=fda214edcfbb359d56462e4911e4f922c2eb466d3b38760cf8b60230731f1b4a

define Build/Prepare/luci-app-voice
	$(STAGING_DIR_HOST)/bin/zstd -dc $(DL_DIR)/$(PKG_SOURCE) | \
		$(TAR) -x -C $(PKG_BUILD_DIR) --strip-components=3 \
			$(PKG_SOURCE_SUBDIR)/luci/luci-app-voice
endef

include $(TOPDIR)/feeds/luci/luci.mk

# Thin package: po/ is extracted from the upstream tarball in Build/Prepare.
# luci.mk only scans ${CURDIR}/po at metadata time, so define zh_Hans
# explicitly and compile it from the build dir.
$(eval $(call LuciTranslation,zh-cn,zh_Hans))

define Package/luci-i18n-voice-zh-cn/install
	$(INSTALL_DIR) $(1)/etc/uci-defaults
	echo "uci set luci.languages.zh_Hans='zh_Hans'; uci commit luci" \
		> $(1)/etc/uci-defaults/luci-i18n-voice-zh-cn
	$(INSTALL_DIR) $(1)$(LUCI_LIBRARYDIR)/i18n
	for po in $(PKG_BUILD_DIR)/po/zh_Hans/*.po; do \
		[ -e "$$$$po" ] || continue; \
		lmo="$$$${po##*/}"; \
		po2lmo "$$$$po" \
			"$(1)$(LUCI_LIBRARYDIR)/i18n/$$$${lmo%.po}.zh-cn.lmo"; \
	done
endef

$(eval $(call BuildPackage,luci-i18n-voice-zh-cn))
