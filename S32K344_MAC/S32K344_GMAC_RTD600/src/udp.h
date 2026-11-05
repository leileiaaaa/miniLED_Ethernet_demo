/*
 * udp.h
 *
 *  Created on: 2021年5月11日
 *      Author: huangfuwang
 */

#ifndef UDP_H_
#define UDP_H_

#include "stdio.h"
#include <stdint.h>

#define FRAME_ETH_DEST_MAC_OFFSET   (0U)
#define FRAME_ETH_SRC_MAC_OFFSET    (6U)
#define FRAME_ETH_TYPE_OFFSET       (12U)   /* EtherType 偏移 */
#define ETHERTYPE_IPV4              (0x0800U)
#define ETHERTYPE_ARP               (0x0806U)

#define FRAME_IP_PROTO_OFFSET       (23U)
#define FRAME_IP_CHECKSUM_OFFSET    (24U)
#define FRAME_IP_SRC_OFFSET	        (26U)
#define FRAME_IP_DEST_OFFSET        (30U)
#define FRAME_IP_PROTO_ICMP         (1U)

#define FRAME_ICMP_TYPE_OFFSET          (34U)
#define FRAME_ICMP_CHECKSUM_OFFSET      (36U)
#define FRAME_ICMP_TYPE_ECHO_REQUEST    (8U)
#define FRAME_ICMP_TYPE_ECHO_REPLY      (0U)

// IPv4 protocol type values
#define ICMP 0x01
#define TCP 0x06
#define UDP 0x11

/* 板卡自己的 IP（按需修改，和 main.c BOARD_IP_ADDR 保持一致） */
#define BOARD_IP  { 192U, 168U, 2U, 10U }

/* ORIGIN IP ADDRESS — UDP 模板的默认值 */
#define ORIGIP1 172
#define ORIGIP2 16
#define ORIGIP3 66
#define ORIGIP4 203

/* DESTINATION IP ADDRESS — UDP 模板的默认值 */
#define DESTIP1 172
#define DESTIP2 16
#define DESTIP3 66
#define DESTIP4 134

/* DESTINATION PORT */
#define DESTPORT 12345

/* ORIGIN PORT */
#define ORIGPORT 2001

#define PACKET_LEN_BEFORE_UDP_PAYLOAD 42

#define ETH_HEADER_LEN                14

#define UDP_HEADER_SIZE               8

/* udp.c 内部的字节拷贝 —— main.c 也在用，这里声明出来 */
void copy_buff(uint8_t *dest, uint8_t *src, uint8_t len);

void buid_udp_packet(uint8_t *data, uint8_t *udp_data, uint16_t data_len);

/**
 * @brief  根据收到的 ICMP Echo Request，原地改造成 Echo Reply 帧
 *         翻转 MAC 和 IP，改 ICMP Type 8→0，重算 ICMP checksum（IP checksum 不变）。
 *         data 必须指向完整以太网帧（含 14B 头 + IPv4 头 + ICMP 头 + payload）。
 * @param  data       完整以太网帧 buffer（原地修改）
 * @param  board_mac  板卡本机 MAC（6 字节）
 * @param  board_ip   板卡本机 IP（4 字节）
 * @param  frame_len  从 802.3 头开始的帧长度（含 IP+ICMP+payload，不含 FCS）
 */
void build_ping_reply_ex(uint8_t *data, const uint8_t *board_mac, const uint8_t *board_ip, uint16_t frame_len);

/**
 * @brief  16-bit checksum（ones-complement）
 * @param  data  数据指针
 * @param  len   字节数（奇数时最后一个字节按 0 补齐）
 */
uint16_t calc_ipv4_checksum(const uint8_t *data, uint16_t len);

#endif /* UDP_H_ */