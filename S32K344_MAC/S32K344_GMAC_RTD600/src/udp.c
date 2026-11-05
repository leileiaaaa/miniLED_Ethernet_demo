/*
 * udp.c
 *
 *  Created on: 2021年5月11日
 *      Author: huangfuwang
 */

#include "udp.h"



uint8_t UDP_packet[] =
{
    /* Starts Ethernet frame */
    0x54, 0x05, 0xdb, 0x08, 0x6d, 0xdf,     // Destination MAC Address
	0x00, 0x11, 0x22, 0x33, 0x44, 0x55,     // Origin MAC Address
    0x08, 0x00,                             // EtherType (IPV4)
    /* Starts IPV4 frame */
    0x45,                                   // Version and Header
    0x00,                                   // Services
    0x00, 0x20,                             // Total Length, whole packet size including header and data
    0xF5, 0x4F,                             // Identification
    0x00, 0x00,                             // Flags
    0x80,                                   // Time To Live
    UDP,                                    // Protocol (UDP)
    0x00, 0x00,                             // IPV4 Header Checksum (calculated by ENET module)
    ORIGIP1, ORIGIP2, ORIGIP3, ORIGIP4,     // Origin IP
    DESTIP1, DESTIP2, DESTIP3, DESTIP4,     // Destination IP
    /* End of IPV4 frame */
    /* Start UDP frame */
    ((ORIGPORT>>8)&0xFF), (ORIGPORT&0xFF),  // Source Port
    ((DESTPORT>>8)&0xFF), (DESTPORT&0xFF),  // Dest Port
    0x00, 0x0C,                             // Length of the UDP frame including header and data
    0x00, 0x00,                             // UDP Checksum of header and data (uses pseudo header with IPV4 frame)
    /* UDP payload goes here */
    /* End of UDP frame */
    0x00, 0x00, 0x00, 0x00                  // Ethernet frame checksum
    /* End of Ethernet frame */
};

void copy_buff(uint8_t *dest, uint8_t *src, uint8_t len)
{
	uint8_t i;

	for (i = 0; i < len; i++)
	{
		dest[i] = src[i];
	}
}

/**
 * @brief  计算 IPv4 / ICMP / TCP / UDP 都用的 16-bit ones-complement checksum
 * @param  data  要 checksum 的数据区
 * @param  len   字节数，奇数字节时最后补 0
 * @return       ones-complement checksum（直接可填回 IP header / ICMP header 的 checksum 字段）
 */
uint16_t calc_ipv4_checksum(const uint8_t *data, uint16_t len)
{
    uint32_t sum = 0U;
    uint16_t i;
    uint16_t word;

    /* 每两字节凑一个 16-bit word，按网络序（大端）累加 */
    for (i = 0U; i + 1U < len; i += 2U)
    {
        word  = ((uint16_t)data[i] << 8U) | (uint16_t)data[i + 1U];
        sum  += word;
    }
    /* 奇数字节：最后一个字节高字节补 0 */
    if (len & 1U)
    {
        sum += ((uint16_t)data[i] << 8U);
    }
    /* 把高 16 位回卷到低 16 位 */
    while (sum >> 16U)
    {
        sum = (sum & 0xFFFFU) + (sum >> 16U);
    }
    return (uint16_t)(~sum);
}

void delay(volatile int cycles)
{
    /* Delay function - do nothing for a number of cycles */
    while(cycles--);
}

void build_ping_reply(uint8_t *data)
{
	//uint8_t src_mac[6]  = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };	/* MAC address of board */
	uint8_t src_mac[6]  = { 0x22, 0x33, 0x44, 0x55, 0x66, 0x77 };	/* MAC address of board */
	//uint8_t src_mac[6]  = { 0x70, 0x4D, 0x7B, 0x66, 0xF2, 0x40 };	/* MAC address of board */
	uint8_t src_ip[4]   = { 192, 168, 1, 10 };												/* 192.168.1.10 IP of board */

	/* Clear IP and ICMP checksums (to be filled in by the MAC) */
	data[FRAME_IP_CHECKSUM_OFFSET] = 0;
	data[FRAME_IP_CHECKSUM_OFFSET + 1] = 0;
	data[FRAME_ICMP_CHECKSUM_OFFSET] = 0;
	data[FRAME_ICMP_CHECKSUM_OFFSET + 1] = 0;

	/* Update ICMP type to represent an echo reply */
	data[FRAME_ICMP_TYPE_OFFSET] = FRAME_ICMP_TYPE_ECHO_REPLY;

	/* Update MAC and IP addresses */
	copy_buff(&data[FRAME_ETH_DEST_MAC_OFFSET], &data[FRAME_ETH_SRC_MAC_OFFSET], 6);
	copy_buff(&data[FRAME_ETH_SRC_MAC_OFFSET], src_mac, 6);
	copy_buff(&data[FRAME_IP_DEST_OFFSET], &data[FRAME_IP_SRC_OFFSET], 4);
	copy_buff(&data[FRAME_IP_SRC_OFFSET], src_ip, 4);
}
void buid_udp_packet(uint8_t *data, uint8_t *udp_data, uint16_t data_len)
{
//	copy_buff(&data[14],udp_data,data_len);
	//data
	copy_buff(&data[FRAME_ETH_DEST_MAC_OFFSET], &UDP_packet[0], 42); // udp_packet header
	/* 整个包的长度  高八位和2低八位 */
	data[16] = ((42 - ETH_HEADER_LEN + data_len + 4) >> 8) & 0xFF;
	data[17] = (42 - ETH_HEADER_LEN + data_len + 4) & 0xFF;  //40
	/*整个帧的长度  高八位和2低八位*/
	data[38] = ((UDP_HEADER_SIZE + data_len) >> 8) & 0xFF;
	data[39] = (UDP_HEADER_SIZE + data_len) & 0xFF; // 16
	// udp_payload
	copy_buff(&data[PACKET_LEN_BEFORE_UDP_PAYLOAD], udp_data, data_len);
}
/**
 * @brief  把收到的 ICMP Echo Request 原地改造成 Echo Reply
 *         入参是完整以太网帧（14B MAC + 20B IP + 8B ICMP + payload）
 * @param  data       帧 buffer（原地修改）
 * @param  board_mac  板卡 MAC
 * @param  board_ip   板卡 IP
 * @param  frame_len  不含 FCS 的帧总长度（= MAC_PKT_LEN - 4，因为 RxInfo.PktLen 含 FCS）
 */
void build_ping_reply_ex(uint8_t *data, const uint8_t *board_mac, const uint8_t *board_ip, uint16_t frame_len)
{
    /* ---- 1. 翻转以太网 MAC ---- */
    /* dest ← 原 src；src ← 板卡 MAC */
    copy_buff(&data[FRAME_ETH_DEST_MAC_OFFSET], &data[FRAME_ETH_SRC_MAC_OFFSET], 6);
    copy_buff(&data[FRAME_ETH_SRC_MAC_OFFSET], (uint8_t *)board_mac, 6);

    /* ---- 2. 翻转 IP 地址 ---- */
    copy_buff(&data[FRAME_IP_DEST_OFFSET], &data[FRAME_IP_SRC_OFFSET], 4);
    copy_buff(&data[FRAME_IP_SRC_OFFSET], (uint8_t *)board_ip, 4);

    /* ---- 3. 改 ICMP Type：8 (Echo Request) → 0 (Echo Reply) ---- */
    data[FRAME_ICMP_TYPE_OFFSET] = FRAME_ICMP_TYPE_ECHO_REPLY;

    /* ---- 4. 重算 ICMP checksum ---- */
    /* ICMP 从偏移 34 开始（14 eth + 20 ip），长度 = frame_len - 34 */
    {
        uint16_t icmp_len = frame_len - FRAME_ICMP_TYPE_OFFSET;
        /* 先清零旧 checksum 再算 */
        data[FRAME_ICMP_CHECKSUM_OFFSET]     = 0;
        data[FRAME_ICMP_CHECKSUM_OFFSET + 1] = 0;
        {
            uint16_t cksum = calc_ipv4_checksum(&data[FRAME_ICMP_TYPE_OFFSET], icmp_len);
            data[FRAME_ICMP_CHECKSUM_OFFSET]     = (cksum >> 8) & 0xFF;
            data[FRAME_ICMP_CHECKSUM_OFFSET + 1] =  cksum       & 0xFF;
        }
    }

    /* IP header checksum 不需要重算！因为我们只改了 IP src/dst，但 src/dst 本身也没改数值（原 src=新 dest, 原 dest=新 src），
     * checksum 对 src/dst 交换是不变的（加法交换律 + ones-complement 也是交换律）。
     * 所以 IP checksum 保持原值是正确的。 */
}

#if 0
void rx_callback(uint8_t instance, enet_event_t event, uint8_t ring)
{
	status_t stat;

	if (event == ENET_RX_EVENT)
	{
		enet_buffer_t rxBuff;

		stat = ENET_DRV_ReadFrame(INST_ETHERNET1, ring, &rxBuff, NULL);

		if (stat == STATUS_SUCCESS)
		{
			if (rxBuff.length < FRAME_ICMP_TYPE_OFFSET)
			{
				ENET_DRV_ProvideRxBuff(INST_ETHERNET1, ring, &rxBuff);
				return;
			}

			/* Check that the received frame was an ICMP echo request */
			if (rxBuff.data[FRAME_IP_PROTO_OFFSET] == FRAME_IP_PROTO_ICMP &&
					rxBuff.data[FRAME_ICMP_TYPE_OFFSET] == FRAME_ICMP_TYPE_ECHO_REQUEST)
			{
				build_ping_reply(rxBuff.data);

				rxBuff.length = rxBuff.length - 4;

				/* Send back an ICMP echo reply frame */
				ENET_DRV_SendFrame(INST_ETHERNET1, 0U, &rxBuff, NULL);
			}

			ENET_DRV_ProvideRxBuff(INST_ETHERNET1, ring, &rxBuff);
		}
	}
}
#endif