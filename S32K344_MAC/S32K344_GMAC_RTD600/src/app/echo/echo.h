#ifndef ECHO_H
#define ECHO_H

#include "lwip/err.h"
#include "lwip/ip4_addr.h"

#define ECHO_PORT_UDP       9999
#define ECHO_PORT_TCP       9999

typedef struct {
    volatile uint32_t udp_rx_packets;
    volatile uint32_t udp_rx_bytes;
    volatile uint32_t udp_tx_packets;
    volatile uint32_t udp_tx_bytes;
    volatile uint32_t udp_err;

    volatile uint32_t tcp_connections;
    volatile uint32_t tcp_rx_packets;
    volatile uint32_t tcp_rx_bytes;
    volatile uint32_t tcp_tx_bytes;
    volatile uint32_t tcp_err;
} echo_stats_t;

extern volatile echo_stats_t g_echo_stats;

void echo_init(void);

#endif /* ECHO_H */