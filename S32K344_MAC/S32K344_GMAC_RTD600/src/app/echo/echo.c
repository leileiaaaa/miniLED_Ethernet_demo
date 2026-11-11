#include "echo.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
#include "lwip/pbuf.h"
#include "lwip/netif.h"
#include "lwip/ip_addr.h"

volatile echo_stats_t g_echo_stats = {0};

static struct udp_pcb *s_udp_pcb = NULL;
static struct tcp_pcb *s_tcp_listen_pcb = NULL;

/* ============================================================
 *  UDP ECHO
 * ============================================================ */

static void udp_echo_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p,
                          const ip_addr_t *addr, u16_t port)
{
    LWIP_UNUSED_ARG(arg);
    if (p == NULL) {
        return;
    }

    g_echo_stats.udp_rx_packets++;
    g_echo_stats.udp_rx_bytes += p->tot_len;

    err_t err = udp_sendto(pcb, p, addr, port);
    if (err != ERR_OK) {
        g_echo_stats.udp_err++;
    } else {
        g_echo_stats.udp_tx_packets++;
        g_echo_stats.udp_tx_bytes += p->tot_len;
    }

    pbuf_free(p);
}

/* ============================================================
 *  TCP ECHO
 * ============================================================ */

struct tcp_echo_priv {
    struct tcp_pcb *pcb;
};

static void tcp_echo_err(void *arg, err_t err)
{
    LWIP_UNUSED_ARG(err);
    struct tcp_echo_priv *priv = (struct tcp_echo_priv *)arg;
    if (priv != NULL) {
        mem_free(priv);
    }
    g_echo_stats.tcp_err++;
}

static err_t tcp_echo_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    if (err != ERR_OK || p == NULL) {
        struct tcp_echo_priv *priv = (struct tcp_echo_priv *)arg;
        if (priv != NULL) {
            mem_free(priv);
        }
        tcp_close(tpcb);
        return ERR_OK;
    }

    g_echo_stats.tcp_rx_packets++;
    g_echo_stats.tcp_rx_bytes += p->tot_len;

    struct pbuf *q;
    for (q = p; q != NULL; q = q->next) {
        u8_t flags = (q->next != NULL) ? TCP_WRITE_FLAG_MORE : 0;
        err_t werr = tcp_write(tpcb, q->payload, q->len, flags);
        if (werr != ERR_OK) {
            g_echo_stats.tcp_err++;
            break;
        }
        g_echo_stats.tcp_tx_bytes += q->len;
    }

    tcp_recved(tpcb, p->tot_len);
    tcp_output(tpcb);
    pbuf_free(p);

    return ERR_OK;
}

static err_t tcp_echo_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    LWIP_UNUSED_ARG(arg);
    if (err != ERR_OK || newpcb == NULL) {
        return ERR_OK;
    }

    struct tcp_echo_priv *priv = (struct tcp_echo_priv *)mem_malloc(sizeof(struct tcp_echo_priv));
    if (priv == NULL) {
        g_echo_stats.tcp_err++;
        tcp_abort(newpcb);
        return ERR_ABRT;
    }
    priv->pcb = newpcb;

    tcp_arg(newpcb, priv);
    tcp_recv(newpcb, tcp_echo_recv);
    tcp_err(newpcb, tcp_echo_err);

    g_echo_stats.tcp_connections++;
    return ERR_OK;
}

/* ============================================================
 *  INIT
 * ============================================================ */

void echo_init(void)
{
    ip_addr_t any;
    ip_addr_set_zero_ip4(&any);

    s_udp_pcb = udp_new();
    if (s_udp_pcb != NULL) {
        if (udp_bind(s_udp_pcb, &any, ECHO_PORT_UDP) == ERR_OK) {
            udp_recv(s_udp_pcb, udp_echo_recv, NULL);
        }
    }

    s_tcp_listen_pcb = tcp_new();
    if (s_tcp_listen_pcb != NULL) {
        if (tcp_bind(s_tcp_listen_pcb, &any, ECHO_PORT_TCP) == ERR_OK) {
            s_tcp_listen_pcb = tcp_listen(s_tcp_listen_pcb);
            if (s_tcp_listen_pcb != NULL) {
                tcp_accept(s_tcp_listen_pcb, tcp_echo_accept);
            }
        }
    }
}