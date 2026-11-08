# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=airoha-voice
PKG_RELEASE:=27

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/Sirherobrine23/airoha_voip.git
PKG_SOURCE_DATE:=2026-09-24
PKG_SOURCE_VERSION:=4222576d980856f3c9ffc323d2fd78a28f94f085
PKG_SOURCE_SUBDIR:=airoha_voip-2026.09.24~4222576d
PKG_SOURCE:=airoha_voip-2026.09.24~4222576d.tar.zst
PKG_MIRROR_HASH:=90a0cd35139c988fcb2c3974b10a9c544eee24f35decab95383a9cccf33bd6ab

PKG_LICENSE:=GPL-2.0-only AND LicenseRef-Proprietary
PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

define KernelPackage/airoha-en7581-pcm-spi
  SUBMENU:=Voice over IP
  TITLE:=Airoha EN7581 single-SLIC voice stack
  DEPENDS:=@TARGET_airoha_an7581_DEVICE_gemtek_xg2010g-ubi||TARGET_airoha_an7581_DEVICE_gemtek_xg2010g-2g-ubi
  FILES:= \
	$(PKG_BUILD_DIR)/src/en75xx-lec.ko \
	$(PKG_BUILD_DIR)/src/en75xx-pcm.ko \
	$(PKG_BUILD_DIR)/src/en75xx-voice.ko \
	$(PKG_BUILD_DIR)/src/en75xx-isi-spi.ko \
	$(PKG_BUILD_DIR)/src/en75xx-slic-si3219x.ko
  AUTOLOAD:=$(call AutoLoad,50,en75xx-lec en75xx-pcm en75xx-voice en75xx-isi-spi en75xx-slic-si3219x)
endef

define KernelPackage/airoha-en7581-pcm-spi/description
 Complete XG2010G EN7581 voice stack: PCM DMA, ISI transport, one Si32192
 ProSLIC FXS line, line control, hook/ring handling and /dev/en75xx-fxs0.
endef

define Package/airoha-voice-ctl
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Telephony
  TITLE:=Airoha EN75xx FXS diagnostic utility
  DEPENDS:=+kmod-airoha-en7581-pcm-spi
endef

define Package/airoha-voice-ctl/description
 Inspect and control EN75xx FXS line state through the stable voice UAPI.
 Read-only commands are suitable for bring-up; ringing and tone generation
 require an explicit command.
endef

define KernelPackage/airoha-en7581-pcm-spi/install
	$(INSTALL_DIR) $(1)/lib/firmware/en75xx/proslic
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/firmware/proslic/si3219x_a_lcqc.fw \
		$(1)/lib/firmware/en75xx/proslic/
endef

define Build/Prepare
	rm -rf $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)
	$(STAGING_DIR_HOST)/bin/zstd -dc $(DL_DIR)/$(PKG_SOURCE) | \
		$(TAR) -x -C $(PKG_BUILD_DIR) --strip-components=1
	$(Build/Patch)
endef

define Build/Compile
	+$(KERNEL_MAKE) $(PKG_JOBS) M="$(PKG_BUILD_DIR)/src" modules
	$(TARGET_CC) $(TARGET_CPPFLAGS) $(TARGET_CFLAGS) \
		-Wall -Wextra -Werror \
		-I$(PKG_BUILD_DIR)/include/uapi \
		-o $(PKG_BUILD_DIR)/airoha-voice-ctl \
		$(CURDIR)/files/airoha-voice-ctl.c \
		$(TARGET_LDFLAGS)
endef

define Package/airoha-voice-ctl/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/airoha-voice-ctl $(1)/usr/sbin/
endef

$(eval $(call KernelPackage,airoha-en7581-pcm-spi))
$(eval $(call BuildPackage,airoha-voice-ctl))
