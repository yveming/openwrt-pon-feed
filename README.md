# openwrt-pon-feed

PonWrt 的外置软件包 feed（XR1710G / XG2010G 产品线）。

主树（`yveming/openwrt-pon` 的 `pon` 分支）只保留官方基座、产品补丁和产品树
自带的包；本 feed 收纳“需要单独打包、但官方基座没有”的包。

## 用法

```sh
# 主树仓库根目录
echo 'src-git pon_feed https://github.com/yveming/openwrt-pon-feed.git' >> feeds.conf
./scripts/feeds update pon_feed
./scripts/feeds install -a
```

## 包清单

| 目录 | 包名 | 来源 | 说明 |
| --- | --- | --- | --- |
| `kernel/airoha-voice` | `kmod-airoha-en7581-pcm-spi`、`airoha-voice-ctl` | `Sirherobrine23/airoha_voip` + 本地补丁 | EN7581 语音（FXS/SLIC/PCM） |
| `net/asterisk-chan-en75xx` | `asterisk-chan-en75xx` | `Sirherobrine23/airoha_voip` + 本地补丁 | Asterisk EN75xx 语音通道 |
| `kernel/nf-nathelper-fcc` | `kmod-nf-nathelper-fcc` | `yveming/nf-nathelper-fcc` | FCC/HW NAT 辅助模块 |
| `iptv/kernel/iptv` | `kmod-iptv` | `yveming/openwrt-iptv` | IPTV 单设备双 VLAN 驱动 |
| `iptv/net/iptv-scripts` | `iptv-scripts` | `yveming/openwrt-iptv` | UCI、init 脚本和 apply 助手 |
| `iptv/luci/luci-app-iptv` | `luci-app-iptv` | `yveming/openwrt-iptv` | IPTV LuCI 页面 |

说明：

- `airoha-voice` 和 `asterisk-chan-en75xx` 使用同一个上游仓库和同一个 commit，
  共用 `dl/airoha_voip-2026.09.24~4222576d.tar.zst`，`PKG_MIRROR_HASH` 相同。
- `nf-nathelper-fcc` 和 `iptv` 是 thin package：feed 里只有 Makefile，源码在
  构建时从上游仓库拉取，不复制进 feed。
- `iptv` 的 LuCI 包用 `Build/Prepare/luci-app-iptv` 从上游 tar 包中提取
  `luci/luci-app-iptv` 子目录。

## 不在本 feed 的包

这些包已经移到更合适的位置，不由本 feed 维护：

| 包 | 位置 | 原因 |
| --- | --- | --- |
| `luci-app-airoha-*`、`luci-app-mesh-conf`、`luci-app-netmode` | `naoki66/luci-app-airoha` feed | 有独立上游仓库 |
| `luci-app-mlo` | 主树 `package/emortal/luci-app-mlo` | naoki66 产品树的一部分，没有独立上游仓库 |
| `airoha-an7581-mt7996-board` | 主树 `package/network/config/` | XR1710G/W1700K 板级配置，没有独立上游仓库 |
| `znxt-zn515-mt7916-eeprom` | 主树 `package/firmware/` | 板级 firmware 数据 |
| `clientstatus` / `luci-app-clientstatus` | 删除 | 未使用，且只有预编译二进制 |

`pon_drivers`、`pon_userspace` 保持独立 feed，不并入本仓库：
它们的构建逻辑（kernel module / Rust）由各自上游维护。

## 维护约定

- feed 里只放打包 Makefile、补丁和少量本地文件；上游源码通过 `PKG_SOURCE`
  获取，不复制进本仓库。
- 需要顺序的补丁用 `series`，补丁文件名优先使用能说明用途的全称；
  内核 target 补丁保留数字前缀。
- 改上游版本时同步更新 `PKG_SOURCE_VERSION` 和 `PKG_MIRROR_HASH`。
- 新增/删除包时同步更新本 README。

## 来源与许可

各包版权归原作者所有，沿用各包内的许可证。本仓库只做集中托管和打包，
便于主树保持干净。
