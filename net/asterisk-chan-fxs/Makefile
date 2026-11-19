# SPDX-License-Identifier: GPL-2.0-only
#
# Generic Asterisk FXS channel driver for fxs-core.

include $(TOPDIR)/rules.mk

PKG_NAME:=asterisk-chan-fxs
PKG_VERSION:=1
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0-only
PKG_MAINTAINER:=ming

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/yveming/airoha-voip.git
PKG_SOURCE_VERSION:=e9735afb7bd3eb5d910ec177f77e2b285b9d67d9
PKG_SOURCE_DATE:=2026-11-19
PKG_SOURCE_SUBDIR:=airoha-voip-2026-11-19~e9735af
PKG_SOURCE:=airoha-voip-2026-11-19~e9735af.tar.zst
PKG_MIRROR_HASH:=056b7079d647af6d9928b2fad4596df938db2b0d339363d3654780f12d76aac7

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)
PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

define Package/asterisk-chan-fxs
  SUBMENU:=Telephony
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+asterisk +kmod-fxs-core
  TITLE:=Generic FXS channel driver
endef

define Package/asterisk-chan-fxs/description
 Asterisk channel driver for fxs-core lines.  It uses /dev/fxsN for
 control and hook events and /dev/fxsN-pcm for 8 kHz signed linear audio.
 The line index and device paths are read from /etc/asterisk/fxs.conf.
endef

define Build/Prepare
	rm -rf $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)
	$(STAGING_DIR_HOST)/bin/zstd -dc $(DL_DIR)/$(PKG_SOURCE) | \
		$(TAR) -x -C $(PKG_BUILD_DIR) --strip-components=3 \
			$(PKG_SOURCE_SUBDIR)/net/asterisk-chan-fxs
	$(CP) $(PKG_BUILD_DIR)/src/. $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(TARGET_CC) -o $(PKG_BUILD_DIR)/chan_fxs.o -c $(PKG_BUILD_DIR)/chan_fxs.c \
		-MD -MT chan_fxs.o -MF $(PKG_BUILD_DIR)/.chan_fxs.o.d -MP -pthread \
		$(TARGET_CFLAGS) -DAST_MODULE_SELF_SYM=__internal_chan_fxs_self \
		$(TARGET_CPPFLAGS) -Wall -Wstrict-prototypes -Wmissing-prototypes \
		-Wmissing-declarations $(FPIC) -DAST_MODULE=\"chan_fxs\"
	$(TARGET_CC) -o $(PKG_BUILD_DIR)/chan_fxs.so -pthread $(TARGET_LDFLAGS) -shared \
		-Wl,--version-script,$(PKG_BUILD_DIR)/chan_fxs.exports,--warn-common \
		$(PKG_BUILD_DIR)/chan_fxs.o -lm
endef

define Package/asterisk-chan-fxs/install
	$(INSTALL_DIR) $(1)/usr/lib/asterisk/modules
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/chan_fxs.so $(1)/usr/lib/asterisk/modules/
	$(INSTALL_DIR) $(1)/etc/asterisk
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/files/fxs.conf $(1)/etc/asterisk/fxs.conf
endef

$(eval $(call BuildPackage,asterisk-chan-fxs))
