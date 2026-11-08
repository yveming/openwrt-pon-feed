# openwrt-pon-feed

PonWrt 的外置软件包 feed（XR1710G / XG2010G 产品线）。

主树（`yveming/openwrt-pon` 的 `pon` 分支）只保留官方基座 + `target/linux`
产品补丁；所有“官方基座没有的包”集中放在这里，按需通过 `feeds.conf.default`
引入。

## 用法

```sh
# 主树仓库根目录
echo 'src-git pon_feed https://github.com/yveming/openwrt-pon-feed.git' >> feeds.conf
./scripts/feeds update pon_feed
./scripts/feeds install -a
```

`configs/*.config` 里的 `CONFIG_PACKAGE_<name>=y` 会照常生效；**feed 没挂上时
这些符号会被 `make defconfig` 静默丢弃**，所以主树的
`scripts/check-seed-symbols.sh` 会在构建前做断言。

## 包清单

| 目录 | 包名 | 用途 | 当前使用位置 |
| --- | --- | --- | --- |
| `emortal/clientstatus` | `clientstatus` | LAN 客户端在线状态采集守护进程 | 未在种子中启用 |
| `emortal/luci-app-clientstatus` | `luci-app-clientstatus` + `luci-i18n-clientstatus-zh-cn` | 客户端状态页面 | 未在种子中启用 |
| `emortal/luci-app-mlo` | `luci-app-mlo` + `luci-i18n-mlo-zh-cn` | Wi-Fi 7 MLO 配置页 | `configs/1710.config` |
| `firmware/znxt-zn515-mt7916-eeprom` | `znxt-zn515-mt7916-eeprom` | ZNXT ZN515XG-D 的 MT7916 EEPROM | `target/linux/airoha/image/an7581.mk` |
| `kernel/airoha-voice` | `kmod-airoha-en7581-pcm-spi`、`airoha-voice-ctl` | EN7581 语音（FXS/SLIC/PCM） | `configs/2010.config`、`2010-2g.config`、`an7581.mk` |
| `kernel/nf-nathelper-fcc` | `kmod-nf-nathelper-fcc` | FCC/HW NAT 辅助模块 | 当前仅本地 `.config` |
| `luci/luci-app-airoha-factory` | `luci-app-airoha-factory` + i18n | Airoha 工厂模式页 | `configs/1710.config` |
| `luci/luci-app-airoha-fancontrol` | `luci-app-airoha-fancontrol` + i18n | 风扇控制页 | `configs/1710.config` |
| `luci/luci-app-airoha-npu` | `luci-app-airoha-npu` + i18n | NPU/PPE/CPU 状态页 | `configs/1710.config`、`2010.config`、`2010-2g.config` |
| `luci/luci-app-airoha-recovery` | `luci-app-airoha-recovery` + i18n | Airoha 恢复页 | `configs/1710.config` |
| `luci/luci-app-mesh-conf` | `luci-app-mesh-conf` + i18n | Mesh 配置页 | `configs/1710.config` |
| `luci/luci-app-netmode` | `luci-app-netmode` + i18n | 网络模式切换页 | `configs/1710.config` |
| `net/airoha-an7581-mt7996-board` | `airoha-an7581-mt7996-board` | XR1710G 无线/PPE 默认配置与 packet-steering | `target/linux/airoha/image/an7581.mk` |
| `net/asterisk-chan-en75xx` | `asterisk-chan-en75xx` | Asterisk 的 EN75xx 语音通道 | `configs/2010.config`、`2010-2g.config` |

说明：

- `kernel/airoha-voice` 同时产出 `kmod-airoha-en7581-pcm-spi` 和
  `airoha-voice-ctl` 两个包。
- LuCI 包的 `po/` 会自动生成 `luci-i18n-*` 子包，表中不逐条列出。
- `emortal/clientstatus` 系列在三个种子配置里都是 `not set`；保留是为了
  某些定制机型，若确认不用可以删除。
- `kmod-nf-nathelper-fcc` 目前只在本地 `.config` 启用，尚未写入机型默认配置。

## 迁移与退役

- `kmod-iptv` / `iptv-scripts` / `luci-app-iptv` 已迁移到独立仓库
  `yveming/openwrt-iptv`，本 feed 不再包含 IPTV 包。
- `pon_userspace` 里的旧桥接版 `luci-app-iptv` 已由主树的
  `patches/feeds/pon_userspace/100-retire-luci-app-iptv.patch` 退役；
  执行 `feeds update` 后该包会被自动删除，不会再进入索引。
- 旧的 `luci-proto-iptv` 本地实现已删除；通用 IPTV 现在不依赖 netifd proto。

## 维护约定

- 每个包一个目录，目录名尽量与包名一致；目录下只放源码、补丁和打包文件，
  不放构建产物。
- 新增/删除包时同步更新本 README 的包清单和使用位置。
- 需要顺序的补丁用 `series`，补丁文件名优先使用能说明用途的全称；
  内核 target 补丁保留数字前缀。
- 上游代码通过 `PKG_SOURCE_*` 固定 commit；本地改动放 `patches/`，
  并在提交信息里写清楚基线。

## 来源与许可

各包版权归原作者所有，沿用各包内的许可证；本仓库只做集中托管，便于主树
保持干净。`kmod-nf-nathelper-fcc` 与 `luci-app-airoha-npu` 分别来自
`yveming/nf-nathelper-fcc`、`rchen14b/luci-app-airoha-npu` 的导入版本。
