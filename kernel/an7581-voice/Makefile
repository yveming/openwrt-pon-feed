# SPDX-License-Identifier: GPL-2.0-only
#
# Thin package for the AN7581 PCM/ZSI/Le9641 source kept in yveming/airoha-voice.

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=an7581-voice
PKG_RELEASE:=4
PKG_LICENSE:=GPL-2.0-only

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/yveming/airoha-voice.git
PKG_SOURCE_VERSION:=376eccb99ae96d842f8e85fd6e5d1c267afe5932
PKG_SOURCE_DATE:=2026-11-11
PKG_SOURCE_SUBDIR:=airoha-voice-2026-11-11~376eccb
PKG_SOURCE:=airoha-voice-2026-11-11~376eccb.tar.zst
PKG_MIRROR_HASH:=fda214edcfbb359d56462e4911e4f922c2eb466d3b38760cf8b60230731f1b4a

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/an7581-voice
PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

PKG_VERSION:=

define KernelPackage/an7581-zsi
  SUBMENU:=Voice over IP
  TITLE:=Airoha AN7581 ZSI transport
  DEPENDS:=@TARGET_airoha_an7581
  FILES:=$(PKG_BUILD_DIR)/an7581-zsi.ko
  # Loaded in dependency order by /etc/init.d/an7581-voice.
  AUTOLOAD:=
endef

define KernelPackage/an7581-zsi/description
 AN7581-only ZSI transport for the HG5582A Le9641.  Exposes
 /proc/an7581-zsi for controlled detection and register access.  The
 module also performs the PCM clock bring-up needed to clock ZSI control
 bytes.
endef

define KernelPackage/an7581-pcm
  SUBMENU:=Voice over IP
  TITLE:=Airoha AN7581 PCM engine
  DEPENDS:=@TARGET_airoha_an7581 +kmod-an7581-zsi
  FILES:=$(PKG_BUILD_DIR)/an7581-pcm.ko
  AUTOLOAD:=
endef

define KernelPackage/an7581-pcm/description
 AN7581 PCM engine with OEM-derived 12-byte/15-descriptor rings, OSLEC
 line echo cancellation and a /proc/an7581-pcm diagnostic interface.
endef

define KernelPackage/an7581-slic-le9641
  SUBMENU:=Voice over IP
  TITLE:=Airoha AN7581 Le9641 SLIC
  DEPENDS:=@TARGET_airoha_an7581 +kmod-an7581-zsi
  FILES:=$(PKG_BUILD_DIR)/an7581-slic-le9641.ko
  AUTOLOAD:=
endef

define KernelPackage/an7581-slic-le9641/description
 Le9641 raw-MPI init, hook/ring control and linear audio path over the
 AN7581 ZSI transport.  Exposes /proc/an7581-slic for diagnostics.
endef

define Build/Prepare
	rm -rf $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)
	$(STAGING_DIR_HOST)/bin/zstd -dc $(DL_DIR)/$(PKG_SOURCE) | \
		$(TAR) -x -C $(PKG_BUILD_DIR) --strip-components=4 \
			$(PKG_SOURCE_SUBDIR)/kernel/an7581-voice/src
	$(Build/Patch)
endef

define Build/Compile
	+$(KERNEL_MAKE) $(PKG_JOBS) M="$(PKG_BUILD_DIR)" modules
endef

$(eval $(call KernelPackage,an7581-zsi))
$(eval $(call KernelPackage,an7581-pcm))
$(eval $(call KernelPackage,an7581-slic-le9641))
