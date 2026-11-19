# SPDX-License-Identifier: Apache-2.0
#
# Thin package for luci-app-voip kept in yveming/airoha-voip.

include $(TOPDIR)/rules.mk

LUCI_TITLE:=LuCI VoIP / SIP configuration
LUCI_DEPENDS:=+luci-base +asterisk +asterisk-chan-fxs \
	+asterisk-pjsip +asterisk-res-pjproject +ip-full
LUCI_PKGARCH:=all
PKG_VERSION:=0.2.2
PKG_RELEASE:=1
PKG_PO_VERSION:=0.2.2
PKG_LICENSE:=Apache-2.0

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/yveming/airoha-voip.git
PKG_SOURCE_VERSION:=e9735afb7bd3eb5d910ec177f77e2b285b9d67d9
PKG_SOURCE_DATE:=2026-11-19
PKG_SOURCE_SUBDIR:=airoha-voip-2026-11-19~e9735af
PKG_SOURCE:=airoha-voip-2026-11-19~e9735af.tar.zst
PKG_MIRROR_HASH:=056b7079d647af6d9928b2fad4596df938db2b0d339363d3654780f12d76aac7

define Build/Prepare/luci-app-voip
	$(STAGING_DIR_HOST)/bin/zstd -dc $(DL_DIR)/$(PKG_SOURCE) | \
		$(TAR) -x -C $(PKG_BUILD_DIR) --strip-components=3 \
			$(PKG_SOURCE_SUBDIR)/luci/luci-app-voip
endef

include $(TOPDIR)/feeds/luci/luci.mk

# Thin package: po/ is extracted from the upstream tarball in Build/Prepare.
# luci.mk only scans ${CURDIR}/po at metadata time, so define zh_Hans
# explicitly and compile it from the build dir.
$(eval $(call LuciTranslation,zh-cn,zh_Hans))

define Package/luci-i18n-voip-zh-cn/install
	$(INSTALL_DIR) $(1)/etc/uci-defaults
	echo "uci set luci.languages.zh_Hans='zh_Hans'; uci commit luci" \
		> $(1)/etc/uci-defaults/luci-i18n-voip-zh-cn
	$(INSTALL_DIR) $(1)$(LUCI_LIBRARYDIR)/i18n
	for po in $(PKG_BUILD_DIR)/po/zh_Hans/*.po; do \
		[ -e "$$$$po" ] || continue; \
		lmo="$$$${po##*/}"; \
		po2lmo "$$$$po" \
			"$(1)$(LUCI_LIBRARYDIR)/i18n/$$$${lmo%.po}.zh-cn.lmo"; \
	done
endef

$(eval $(call BuildPackage,luci-i18n-voip-zh-cn))
