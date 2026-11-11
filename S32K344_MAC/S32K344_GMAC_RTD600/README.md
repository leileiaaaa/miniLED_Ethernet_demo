# S32K344 GMAC + lwIP Ethernet Demo

基于 NXP S32K344 MCU 的嵌入式以太网示例工程，使用 S32K3 RTD 6.0.0 GMAC 驱动 + lwIP V2.x 协议栈 (NO_SYS 无操作系统模式，raw API)。

## 硬件配置

| 项目 | 值 |
|------|-----|
| MCU | S32K344 (Arm Cortex-M7, 400MHz) |
| MAC | GMAC (10/100/1000 Mbps, DMA 收发) |
| 板子 IP | `192.168.2.10` |
| 板子 MAC | `66:55:44:33:22:11` |
| PC 网段 | `192.168.2.x` （同网段直连或交换机） |

## 目录结构

```
S32K344_GMAC_RTD600/
├── .settings/                  # S32DS 工程配置
├── board/                      # 板级引脚配置 (MEX 生成)
├── Project_Settings/           # 启动文件、链接脚本、芯片配置
│   ├── Linker_Files/
│   │   └── linker_flash_s32k344.ld    # 关键！no-cache SRAM 64KB 限制
│   └── Startup_Code/
│       └── startup_cm7.s               # 启动代码 (SRAM_LOOP BSS 清零)
├── RTD/                        # NXP 官方 S32K3 RTD 6.0.0 组件 (只读)
│   ├── src/                    # Gmac_Ip.c, OsIf.c, Clock_Ip.c ...
│   └── include/
├── generate/                   # MEX 代码生成文件
│   ├── src/
│   │   ├── Gmac_Ip_Cfg.c              # GMAC buffer/descriptor 数组定义
│   │   └── Gmac_Ip_Sa_PBcfg.c         # GMAC ring 配置 (ringSize 硬编码)
│   └── include/
│       ├── Gmac_Ip_Sa_PBcfg.h         # GMAC_MAX_TXBUFF/RXBUFF 宏
│       └── ...
├── src/
│   ├── app/
│   │   ├── main.c                     # 主程序：时钟→GMAC→lwIP→loop
│   │   └── echo/
│   │       ├── echo.c                 # UDP/TCP echo 服务 (raw API)
│   │       └── echo.h                 # g_echo_stats 统计结构
│   ├── middleware/
│   │   └── lwip/
│   │       ├── ethernetif.c           # GMAC <-> lwIP netif 适配层
│   │       ├── ethernetif.h
│   │       └── arch/
│   │           ├── cc.h               # lwIP CPU/编译器适配
│   │           └── perf.h             # 性能计时适配
│   ├── config/
│   │   └── lwipopts.h                 # lwIP 配置 (PBUF_POOL_SIZE=24, 软件校验和)
│   ├── bsp/                           # (预留) 时钟/引脚初始化封装
│   ├── driver/                        # (预留) 自研硬件驱动
│   └── platform/                      # (预留) 平台公共接口
├── third_party/
│   └── lwip/src/                      # lwIP V2.x 原生协议栈 (只读)
│       ├── api/
│       ├── core/
│       ├── include/
│       └── netif/
├── test/
│   └── udp_perf/
├── tools/
│   ├── powershell/                    # PC 端测试脚本
│   │   ├── diag_test.ps1              # 综合诊断 (ARP + ping + UDP probe)
│   │   ├── udp_echo_test.ps1          # 双向 Echo 回环性能测试
│   │   └── udp_tx_stress.ps1          # 板子 RX 吞吐压测 (PC 纯发)
│   └── python/
├── Debug_FLASH/                       # 编译输出 (Clean 时删除)
├── .cproject / .project / .default_mex
└── .gitignore
```

## 构建与下载

### 1. 导入工程
- S32DS File -> Import -> Existing Projects into Workspace
- 选择本目录根路径

### 2. Clean + Build
```
Project -> Clean... -> OK
Ctrl+B (Build)
```
输出：`Debug_FLASH/S32K344_GMAC_RTD600.elf`

### 3. 下载调试
- Debug 配置选择 S32K344 -> S32DS 默认 PeMicro Link
- Flash 编程后直接 Run

## 网络测试方法

所有 PC 端脚本在 `tools/powershell/` 下执行，需要以管理员权限运行（ARP 操作需要）：

```powershell
cd D:\...\S32K344_GMAC_RTD600\tools\powershell

# 1. 综合诊断 -- 快速确认板子活着
.\diag_test.ps1
# 检查点: ARP OK -> ping OK -> UDP probe OK

# 2. 双向 Echo 回环测试
.\udp_echo_test.ps1
# 10 秒双向 UDP，0% 丢包为正常

# 3. RX 吞吐压测（PC 纯发，板子只收）
.\udp_tx_stress.ps1
# PC 发送速率约 90Mbps，真实板子 RX 吞吐看调试器侧 g_echo_stats
```

### 调试器侧统计（S32DS Expressions 窗口）

```
g_echo_stats.udp_rx_bytes     // 收到的总字节数
g_echo_stats.udp_rx_packets   // 收到的总包数
g_echo_stats.udp_tx_packets   // echo 回发包数
g_echo_stats.udp_err          // 错误计数（应为 0）
```

真实 RX 吞吐 = `g_echo_stats.udp_rx_bytes / 测试秒数 * 8`

## 性能基线

| 测试 | 结果 | 说明 |
|------|------|------|
| ICMP ping | RT 1~2ms, 0% 丢包 | 正常 |
| UDP Echo 双向 | ~4.39 Mbps, 0% 丢包 | 瓶颈：NO_SYS 单线程 + 软件校验和 |
| 板子 RX 上限 | 实测 ~27 Mbps | ring=16, GMAC DMA + lwIP raw API |

## 关键实现细节

### lwIP 配置 (config/lwipopts.h)
- `NO_SYS=1`：无操作系统模式，单线程 raw API
- `CHECKSUM_BY_HARDWARE=0`：必须关闭！GMAC RTD 未启用硬件 checksum 插入，开了会导致 ping 不通（ICMP checksum=0）
- `PBUF_POOL_SIZE=24`，`PBUF_POOL_BUFSIZE=1536`

### GMAC ring 配置（三处必须对齐）
| 位置 | 值 | 含义 |
|------|-----|------|
| `generate/include/Gmac_Ip_Sa_PBcfg.h` | `GMAC_0_MAX_TXBUFF/RXBUFF = 16U` | DMA buffer 数组大小 |
| `generate/src/Gmac_Ip_Sa_PBcfg.c` | `.ringSize = 16U` | GMAC 硬件 ring 描述符数 |
| `linker_flash_s32k344.ld` | `int_sram_no_cacheable = 64KB` | DMA buffer 物理地址区 |

不要只改 PBcfg.h 宏！MEX 生成的配置结构 `.ringSize` 是硬编码的，必须同步改。

### linker 脚本 SRAM 布局
```
物理 SRAM 上限: 0x20443FFF (注释说 48KB 被固件阻断，实际可用 112KB)
+-----------------------+----------+----------------------------------+
| 区域                   | 大小      | 用途                               |
+-----------------------+----------+----------------------------------+
| int_sram_no_cacheable | 64KB     | GMAC DMA RX/TX buffer + desc      |
| int_sram_results      | 256B     | RTD 内部寄存器暂存                 |
| int_sram_shareable    | 16KB     | 通用共享 SRAM                      |
+-----------------------+----------+----------------------------------+
```

GMAC ring 32 x 1536 x 2 = 96KB > 64KB no-cache 区，所以 ring 必须配 16。

### ethernetif.c 发送路径
```
low_level_output() 简化流程:
  GetTxBuff() -> memcpy -> SendFrame() -> 直接返回
  (已删除 busy-wait，否则 TX 阻塞会导致 RX ring 耗尽)
```

## 已知限制 & 待优化

1. RX ring=16：如果要更大 ring，需要给 GMAC DMA buffer 启用 cache 管理（`GMAC_HAS_CACHE_MANAGEMENT`），把 buffer 放在 cacheable SRAM + 做 cache invalidation/clean，就能摆脱 64KB 限制
2. 软件校验和：开启 `GMAC_CHECKSUM_INSERTION_ENABLE` + RX checksum offload，可大幅降低 CPU 负载
3. NO_SYS 单线程：RTX/FreeRTOS + 中断驱动 + `tcpip_input()` 可提升吞吐
4. TX 无 descriptor 回收：当前依赖 GMAC 硬件自动释放，压力场景需监控 TX ring 状态

## 常见问题

| 现象 | 原因 | 解决 |
|------|------|------|
| Ping 不通，Wireshark 见 ICMP checksum=0 | CHECKSUM_BY_HARDWARE 打开但 GMAC 未启用硬件校验和 | lwipopts.h 设 0，开全软件校验和 |
| Linker overflow 37KB | GMAC ring=32 的 DMA buffer 96KB > 64KB no-cache 区 | ring 改 16（三处同步！） |
| SRAM_LOOP 启动卡死 | linker script 超出物理 SRAM 上限 0x20443FFF | 还原 ram_rsvd2 到 0x20444000 |
| 板子跑几秒挂 | TX busy-wait 阻塞主循环，RX ring 耗尽 -> DMA 写非法地址 | ethernetif.c 删 busy-wait |
| Windows socket buffer 溢出 | Windows UDP 接收缓冲默认 8KB | PowerShell 脚本设 1MB |