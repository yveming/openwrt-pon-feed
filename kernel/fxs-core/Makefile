# SPDX-License-Identifier: GPL-2.0-only
#
# Generic FXS line core: one line object, one control device and one PCM
# device per line.  Board/SLIC/PCM providers register lines with the core.

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=fxs-core
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0-only
PKG_MAINTAINER:=ming

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/yveming/airoha-voip.git
PKG_SOURCE_VERSION:=5617f943403eb3ab900ab106ef5324eae698d848
PKG_SOURCE_DATE:=2026-11-19
PKG_SOURCE_SUBDIR:=airoha-voip-2026-11-19~5617f94
PKG_SOURCE:=airoha-voip-2026-11-19~5617f94.tar.zst
PKG_MIRROR_HASH:=f95be3b821f55b8cb6c59e2532569bfde7fd0bf846d1bfa8d6f8537b669ad72f

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/fxs-core
PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

PKG_VERSION:=

define KernelPackage/fxs-core
  SUBMENU:=Voice over IP
  TITLE:=Generic FXS line core
  FILES:=$(PKG_BUILD_DIR)/fxs-core.ko
  AUTOLOAD:=
endef

define KernelPackage/fxs-core/description
 Generic FXS line abstraction.  A provider registers a line with a SLIC
 backend and a PCM backend; the core exposes /dev/fxsN (control/events)
 and /dev/fxsN-pcm (8 kHz s16le audio) per line.
endef

define KernelPackage/fxs-dummy
  SUBMENU:=Voice over IP
  TITLE:=FXS dummy line (testing)
  DEPENDS:=+kmod-fxs-core
  FILES:=$(PKG_BUILD_DIR)/fxs-dummy.ko
  AUTOLOAD:=$(call AutoLoad,50,fxs-dummy)
endef

define KernelPackage/fxs-dummy/description
 A software-only FXS line used to exercise the fxs-core UAPI without
 hardware.  Not intended for production images.
endef

define Build/Prepare
	rm -rf $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)
	$(STAGING_DIR_HOST)/bin/zstd -dc $(DL_DIR)/$(PKG_SOURCE) | \
		$(TAR) -x -C $(PKG_BUILD_DIR) --strip-components=4 \
			$(PKG_SOURCE_SUBDIR)/kernel/fxs-core/src
endef

define Build/Compile
	+$(KERNEL_MAKE) $(PKG_JOBS) M="$(PKG_BUILD_DIR)" modules
endef

$(eval $(call KernelPackage,fxs-core))
$(eval $(call KernelPackage,fxs-dummy))
