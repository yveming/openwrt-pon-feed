# openwrt-pon-feed

PonWrt 的外置软件包 feed（XR1710G / XG2010G 产品线）。

主树（`yveming/openwrt-pon` 的 `pon` 分支）只保留官方基座 + `target/linux` 产品补丁，
所有"官方基座没有的包"集中放在这里，按需通过 `feeds.conf.default` 引入。

## 用法

```sh
# 主树仓库根目录
echo 'src-git pon_feed https://github.com/yveming/openwrt-pon-feed.git' >> feeds.conf.default
./scripts/feeds update -a
./scripts/feeds install -a
```

`configs/*.config` 里的 `CONFIG_PACKAGE_<name>=y` 会照常生效；**feed 没挂上时这些符号会被
`make defconfig` 静默丢弃**，所以主树的 `scripts/check-seed-symbols.sh` 会在构建前做断言。

## 包清单

| 路径 | 包名 | 用途 |
| --- | --- | --- |
| `luci/luci-app-airoha-npu` | `luci-app-airoha-npu` | Airoha NPU/PPE/CPU 状态页（rchen14b 1.0.1，PKG_RELEASE 2） |
| `kernel/nf-nathelper-fcc` | `kmod-nf-nathelper-fcc` | FCC/HW NAT 辅助模块（`yveming/nf-nathelper-fcc`） |
| `kernel/airoha-voice` | `airoha-voice-ctl` | EN7581 语音（FXS/SLIC/DSP），XG2010G VOIP |
| `net/asterisk-chan-en75xx` | `asterisk-chan-en75xx` | Asterisk 的 EN75xx 语音通道 |
| `net/airoha-an7581-mt7996-board` | `airoha-an7581-mt7996-board` | XR1710G 无线/PPE 默认配置与 packet-steering |
| `emortal/clientstatus` | `clientstatus` | 客户端状态采集守护进程 |
| `emortal/luci-app-clientstatus` | `luci-app-clientstatus` | 客户端状态 LuCI 页面 |
| `emortal/luci-i18n-clientstatus-zh-cn` | `luci-i18n-clientstatus-zh-cn` | 上者的中文翻译 |
| `emortal/luci-app-mlo` | `luci-app-mlo` | Wi-Fi 7 MLO 配置页（仅 XR1710G） |
| `firmware/znxt-zn515-mt7916-eeprom` | `znxt-zn515-mt7916-eeprom` | ZNXT ZN515XG-D 的 MT7916 EEPROM |

## 来源与许可

各包版权归原作者所有，沿用各包内的许可证；本仓库只做集中托管，便于主树保持干净。
`kmod-nf-nathelper-fcc` 与 `luci-app-airoha-npu` 分别来自 `yveming/nf-nathelper-fcc`、
`rchen14b/luci-app-airoha-npu` 的导入版本。
