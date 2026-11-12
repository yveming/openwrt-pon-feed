# SPDX-License-Identifier: GPL-2.0-only
#
# Thin package for the Asterisk AN7581 channel driver kept in
# yveming/airoha-voip.

include $(TOPDIR)/rules.mk

PKG_NAME:=asterisk-chan-an7581
PKG_RELEASE:=11
PKG_LICENSE:=GPL-2.0

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/yveming/airoha-voip.git
PKG_SOURCE_VERSION:=786f5deacf75d58b538851c62ffd3e8b6616981a
PKG_SOURCE_DATE:=2026-11-12
PKG_SOURCE_SUBDIR:=airoha-voip-2026-11-12~786f5de
PKG_SOURCE:=airoha-voip-2026-11-12~786f5de.tar.zst
PKG_MIRROR_HASH:=dc0a4fb7053e64412dfc051d1db388055e3954a3f5ca551bb9f0567ecf1a1079

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

PKG_VERSION:=

define Package/asterisk-chan-an7581
  SUBMENU:=Telephony
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+asterisk +kmod-an7581-zsi +kmod-an7581-pcm +kmod-an7581-slic-le9641
  TITLE:=AN7581 Le9641 FXS channel driver
endef

define Package/asterisk-chan-an7581/description
 Asterisk channel driver for the AN7581 PCM/ZSI Le9641 FXS line.
endef

define Build/Prepare
	rm -rf $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)
	$(STAGING_DIR_HOST)/bin/zstd -dc $(DL_DIR)/$(PKG_SOURCE) | \
		$(TAR) -x -C $(PKG_BUILD_DIR) --strip-components=3 \
			$(PKG_SOURCE_SUBDIR)/net/asterisk-chan-an7581
	cp -a $(PKG_BUILD_DIR)/src/. $(PKG_BUILD_DIR)/
	$(Build/Patch)
endef

define Build/Compile
	$(TARGET_CC) -o $(PKG_BUILD_DIR)/chan_an7581.o -c $(PKG_BUILD_DIR)/chan_an7581.c \
		-MD -MT chan_an7581.o -MF $(PKG_BUILD_DIR)/.chan_an7581.o.d -MP -pthread \
		$(TARGET_CFLAGS) -DAST_MODULE_SELF_SYM=__internal_chan_an7581_self \
		$(TARGET_CPPFLAGS) -Wall -Wstrict-prototypes -Wmissing-prototypes \
		-Wmissing-declarations $(FPIC) -DAST_MODULE=\"chan_an7581\"
	$(TARGET_CC) -o $(PKG_BUILD_DIR)/chan_an7581.so -pthread $(TARGET_LDFLAGS) -shared \
		-Wl,--version-script,$(PKG_BUILD_DIR)/chan_an7581.exports,--warn-common \
		$(PKG_BUILD_DIR)/chan_an7581.o -lm
endef

define Package/asterisk-chan-an7581/install
	$(INSTALL_DIR) $(1)/usr/lib/asterisk/modules
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/chan_an7581.so $(1)/usr/lib/asterisk/modules/
	$(INSTALL_DIR) $(1)/etc/asterisk
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/files/an7581.conf $(1)/etc/asterisk/an7581.conf
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/an7581-voice.init $(1)/etc/init.d/an7581-voice
endef

$(eval $(call BuildPackage,asterisk-chan-an7581))
