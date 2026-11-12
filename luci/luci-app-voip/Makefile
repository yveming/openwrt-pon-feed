# SPDX-License-Identifier: Apache-2.0
#
# Thin package for luci-app-voip kept in yveming/airoha-voip.

include $(TOPDIR)/rules.mk

LUCI_TITLE:=LuCI VoIP / SIP configuration
LUCI_DEPENDS:=+luci-base +asterisk +asterisk-chan-an7581 \
	+asterisk-pjsip +asterisk-res-pjproject +ip-full
LUCI_PKGARCH:=all
PKG_VERSION:=0.2.0
PKG_RELEASE:=10
PKG_PO_VERSION:=0.2.0
PKG_LICENSE:=Apache-2.0

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/yveming/airoha-voip.git
PKG_SOURCE_VERSION:=3f5de942cf9ffdc159e5061f11f6c2e6d527ecd6
PKG_SOURCE_DATE:=2026-11-12
PKG_SOURCE_SUBDIR:=airoha-voip-2026-11-12~3f5de94
PKG_SOURCE:=airoha-voip-2026-11-12~3f5de94.tar.zst
PKG_MIRROR_HASH:=d698ee2ec30c6a8403dc77533aa3bacc976ccf6fbd8a7b8cc64f4fd2956e6ee5

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
