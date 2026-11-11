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
PKG_SOURCE_VERSION:=a53a8a1b7e427d1a8cdd091d97854e35c292cfea
PKG_SOURCE_DATE:=2026-11-11
PKG_SOURCE_SUBDIR:=airoha-voice-2026-11-11~a53a8a1
PKG_SOURCE:=airoha-voice-2026-11-11~a53a8a1.tar.zst
PKG_MIRROR_HASH:=7b99d6f83cf74f4da0ed68741be2845fbb0d5ee5be8bc3e4b0dc4d23072fea66

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
