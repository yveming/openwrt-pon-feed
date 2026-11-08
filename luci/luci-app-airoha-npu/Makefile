# SPDX-License-Identifier: Apache-2.0
#
# Thin package for luci-app-airoha-npu kept in rchen14b/luci-app-airoha-npu.
#
# The upstream repository is not laid out as an OpenWrt feed: the package
# payload is at the repository root and the in-tree Makefile lives in a
# subdirectory with a stale copy. Keep the packaging here and extract the
# root htdocs/, root/ and po/ trees from the pinned tarball.

include $(TOPDIR)/rules.mk

LUCI_TITLE:=LuCI Airoha SoC Status (NPU, CPU, Frame Engine)
LUCI_MAINTAINER:=Ryan Chen <rchen14b@gmail.com>
LUCI_URL:=https://github.com/rchen14b/luci-app-airoha-npu
LUCI_DESCRIPTION:=Status page for Airoha EN7581 and AN7583 SoCs: NPU state and \
	reserved memory, the Frame Engine port counters and PSE queues, the PPE \
	flow offload table, plus CPU frequency, governor and direct PLL overclock. \
	Register access for the Frame Engine and overclock sections uses /dev/mem \
	and the busybox devmem applet, which this package enables. PPE needs \
	CONFIG_KERNEL_DEBUG_FS; the WiFi queue view needs the mt76 debugfs nodes.
LUCI_DEPENDS:=+luci-base @TARGET_airoha \
	+@KERNEL_DEVMEM +@BUSYBOX_DEFAULT_DEVMEM +@BUSYBOX_CONFIG_DEVMEM
LUCI_PKGARCH:=all

PKG_NAME:=luci-app-airoha-npu
PKG_VERSION:=1.0.1
PKG_RELEASE:=2
PKG_LICENSE:=Apache-2.0

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/rchen14b/luci-app-airoha-npu.git
PKG_SOURCE_VERSION:=14521b8414da1e98517a295d8ec267087c7dde8e
PKG_SOURCE_DATE:=2026-09-19
PKG_SOURCE_SUBDIR:=luci-app-airoha-npu-2026.09.19~14521b8
PKG_SOURCE:=luci-app-airoha-npu-2026.09.19~14521b8.tar.zst
PKG_MIRROR_HASH:=769f4ca0d36e9ef42210ec9bf577ec9c2dbb8e5f31a778da8bbaea63259df2c7

define Build/Prepare/luci-app-airoha-npu
	$(STAGING_DIR_HOST)/bin/zstd -dc $(DL_DIR)/$(PKG_SOURCE) | \
		$(TAR) -x -C $(PKG_BUILD_DIR) --strip-components=1 \
			$(PKG_SOURCE_SUBDIR)/htdocs \
			$(PKG_SOURCE_SUBDIR)/root \
			$(PKG_SOURCE_SUBDIR)/po
endef

include $(TOPDIR)/feeds/luci/luci.mk

# call BuildPackage: feed scanner marker; luci.mk generates the package rules.
