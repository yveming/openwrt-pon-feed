# SPDX-License-Identifier: GPL-2.0-only
#
# Thin OpenWrt package for the FCC netfilter NAT helper.
#
# The source is fetched from yveming/nf-nathelper-fcc at build time; this
# directory only carries the packaging metadata and the firewall helper
# integration.

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=fcc-nathelper
PKG_VERSION:=1.0.0
PKG_RELEASE:=10

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/yveming/nf-nathelper-fcc.git
PKG_SOURCE_VERSION:=ba6bcfd0acb9ba51cb3c635fc15665c2ea01a248
PKG_SOURCE_DATE:=2026-10-12
PKG_MIRROR_HASH:=77cf8c2c7832bdd3471b43b673ccd326267d548077517beed64320dd829f7969

PKG_MAINTAINER:=ming
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define KernelPackage/nf-nathelper-fcc
  SUBMENU:=Netfilter Extensions
  TITLE:=FCC (Fast Channel Change) NAT helper
  DEPENDS:=+kmod-nf-conntrack +kmod-nf-nat
  FILES:=$(PKG_BUILD_DIR)/src/nf_conntrack_fcc.ko \
	$(PKG_BUILD_DIR)/src/nf_nat_fcc.ko
  AUTOLOAD:=$(call AutoLoad,45,nf_conntrack_fcc nf_nat_fcc)
endef

define KernelPackage/nf-nathelper-fcc/description
 Conntrack and NAT helpers for IPTV Fast Channel Change protocol
 (Huawei and Telecom variants).
endef

define Build/Compile
	$(KERNEL_MAKE) M="$(PKG_BUILD_DIR)/src" modules
endef

# Register the fcc helper in fw4's helper database and accept
# helper-expected media in the forward chain. postinst must be
# IPKG_INSTROOT-aware: at image build time OpenWrt runs it against the
# rootfs staging dir, so the helper section is baked into the image;
# modprobe/firewall reload only run on the device. prerm removes the
# section again by name (apk del only deletes files the package owns,
# the appended block would otherwise survive uninstall). Helper
# rendering is gated on the module being loaded, so a missing module
# simply hides the rules instead of breaking the firewall.
define Package/kmod-nf-nathelper-fcc/postinst
#!/bin/sh
HELPERS="$$IPKG_INSTROOT/usr/share/firewall4/helpers"
[ -f "$$HELPERS" ] || exit 0

if ! grep -q "option name 'fcc'" "$$HELPERS"; then
	# exactly one blank line between sections: strip trailing blank
	# lines, make sure the file ends with a newline, then append
	while [ -z "$$(tail -n 1 "$$HELPERS")" ]; do sed -i '$$d' "$$HELPERS"; done
	[ -n "$$(tail -c 1 "$$HELPERS")" ] && echo >> "$$HELPERS"
	cat >> "$$HELPERS" <<'EOF'

config helper
	option name 'fcc'
	option description 'FCC connection tracking'
	option module 'nf_conntrack_fcc'
	option family 'ipv4'
	option proto 'udp'
	option port '8027'
EOF
fi

if [ -z "$$IPKG_INSTROOT" ]; then
	modprobe nf_conntrack_fcc 2>/dev/null
	/etc/init.d/firewall reload >/dev/null 2>&1 &
fi
exit 0
endef

define Package/kmod-nf-nathelper-fcc/prerm
#!/bin/sh
[ -n "$$IPKG_INSTROOT" ] && exit 0

HELPERS="/usr/share/firewall4/helpers"
if [ -f "$$HELPERS" ] && grep -q "option name 'fcc'" "$$HELPERS"; then
	sed -i '/./{H;$$!d};x;/option name .fcc./d' "$$HELPERS"
	# do not leave a trailing blank line behind
	while [ -z "$$(tail -n 1 "$$HELPERS")" ]; do sed -i '$$d' "$$HELPERS"; done
	sed -i '1{/^$/d}' "$$HELPERS"
	/etc/init.d/firewall reload >/dev/null 2>&1
fi
exit 0
endef

$(eval $(call KernelPackage,nf-nathelper-fcc))
