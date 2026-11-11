from scapy.all import *

BOARD_MAC = "66:55:44:33:22:11"   # ← 替换成板卡 MAC
IFACE = "以太网"                    # ← 替换成你的网卡名（ipconfig 查）

# 构造原始二层帧：Dst=板卡，Src=本机，EtherType=0x0800，payload=0..45
pkt = Ether(dst=BOARD_MAC,
            src=get_if_hwaddr(IFACE),
            type=0x0800) / Raw(load=bytes(range(46)))

print(f"发送帧到 {BOARD_MAC} ...")
sendp(pkt, iface=IFACE, count=5, inter=0.5)   # 连发 5 帧，间隔 0.5s
print("完成")
