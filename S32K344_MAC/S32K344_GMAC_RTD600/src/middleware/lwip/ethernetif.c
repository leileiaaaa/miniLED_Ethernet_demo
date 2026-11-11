/* lwIP NO_SYS ethernet interface for the S32K344 GMAC RTD driver. */

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/stats.h"
#include "lwip/snmp.h"
#include "lwip/etharp.h"
#include "lwip/ethip6.h"
#include "lwip/netif.h"
#include "netif/ethernet.h"

#include "Gmac_Ip.h"
#include "Gmac_Ip_Sa_PBcfg.h"
#include "ethernetif.h"

#define IFNAME0                    'e'
#define IFNAME1                    'n'
#define GMAC_INSTANCE              (0U)
#define GMAC_RING                  (0U)
#define GMAC_FRAME_BUFFER_SIZE     (1536U)
#define GMAC_RX_BATCH_SIZE         (16U)
#define GMAC_PHY_BMSR              (1U)
#define GMAC_PHY_LINK_STATUS       (0x0004U)

struct ethernetif {
  uint8 mac[ETH_HWADDR_LEN];
  Gmac_Ip_BufferType rx_buffer;
  Gmac_Ip_RxInfoType rx_info;
};

static struct ethernetif ethernetif_state;
static Gmac_Ip_BufferType tx_pending[GMAC_0_MAX_TXBUFF_SUPPORTED];
static uint16 tx_pending_count;
static Gmac_Ip_TxOptionsType ethernetif_tx_options = {
  TRUE, GMAC_CRC_AND_PAD_INSERTION, GMAC_CHECKSUM_INSERTION_DISABLE
};

static void reclaim_tx_buffers(struct netif *netif)
{
  uint16 i = 0U;

  LWIP_UNUSED_ARG(netif);
  while (i < tx_pending_count) {
    Gmac_Ip_TxInfoType tx_info = {0};
    Gmac_Ip_StatusType status =
      Gmac_Ip_GetTransmitStatus(GMAC_INSTANCE, GMAC_RING,
                                &tx_pending[i], &tx_info);

    if (status == GMAC_STATUS_SUCCESS) {
      if (tx_info.ErrMask != 0U) {
        LINK_STATS_INC(link.err);
        MIB2_STATS_NETIF_INC(netif, ifouterrors);
      }

      tx_pending_count--;
      tx_pending[i] = tx_pending[tx_pending_count];
    } else {
      if (status != GMAC_STATUS_BUSY) {
        LINK_STATS_INC(link.err);
        MIB2_STATS_NETIF_INC(netif, ifouterrors);
      }
      i++;
    }
  }
}

static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
  struct pbuf *q;
  Gmac_Ip_BufferType tx_buffer;
  Gmac_Ip_StatusType status;
  u16_t copied = 0U;

  if ((p == NULL) || (p->tot_len > GMAC_FRAME_BUFFER_SIZE)) {
    LINK_STATS_INC(link.err);
    MIB2_STATS_NETIF_INC(netif, ifoutdiscards);
    return ERR_BUF;
  }

  reclaim_tx_buffers(netif);
  if (tx_pending_count >= GMAC_0_MAX_TXBUFF_SUPPORTED) {
    LINK_STATS_INC(link.memerr);
    MIB2_STATS_NETIF_INC(netif, ifoutdiscards);
    return ERR_MEM;
  }

  tx_buffer.Data = NULL_PTR;
  tx_buffer.Length = GMAC_FRAME_BUFFER_SIZE;
  status = Gmac_Ip_GetTxBuff(GMAC_INSTANCE, GMAC_RING, &tx_buffer, NULL_PTR);
  if (status != GMAC_STATUS_SUCCESS) {
    LINK_STATS_INC(link.memerr);
    MIB2_STATS_NETIF_INC(netif, ifoutdiscards);
    return ERR_MEM;
  }

  for (q = p; q != NULL; q = q->next) {
    MEMCPY(&tx_buffer.Data[copied], q->payload, q->len);
    copied = (u16_t)(copied + q->len);
  }
  tx_buffer.Length = copied;

  status = Gmac_Ip_SendFrame(GMAC_INSTANCE, GMAC_RING,
                             &tx_buffer, &ethernetif_tx_options);
  if (status != GMAC_STATUS_SUCCESS) {
    LINK_STATS_INC(link.err);
    MIB2_STATS_NETIF_INC(netif, ifouterrors);
    return ERR_IF;
  }

  tx_pending[tx_pending_count] = tx_buffer;
  tx_pending_count++;

  MIB2_STATS_NETIF_ADD(netif, ifoutoctets, copied);
  if (((u8_t *)p->payload)[0] & 1U) {
    MIB2_STATS_NETIF_INC(netif, ifoutnucastpkts);
  } else {
    MIB2_STATS_NETIF_INC(netif, ifoutucastpkts);
  }
  LINK_STATS_INC(link.xmit);
  return ERR_OK;
}

static struct pbuf *low_level_input(struct netif *netif)
{
  struct ethernetif *state = (struct ethernetif *)netif->state;
  struct pbuf *p;
  struct pbuf *q;
  Gmac_Ip_StatusType status;
  u16_t frame_length;
  u16_t copied = 0U;

  status = Gmac_Ip_ReadFrame(GMAC_INSTANCE, GMAC_RING,
                             &state->rx_buffer, &state->rx_info);
  if (status != GMAC_STATUS_SUCCESS) {
    return NULL;
  }

  frame_length = (state->rx_info.PktLen > 4U) ?
                 (u16_t)(state->rx_info.PktLen - 4U) : 0U;
  if ((frame_length < 14U) || (frame_length > GMAC_FRAME_BUFFER_SIZE) ||
      (state->rx_info.ErrMask != 0U)) {
    Gmac_Ip_ProvideRxBuff(GMAC_INSTANCE, GMAC_RING, &state->rx_buffer);
    LINK_STATS_INC(link.drop);
    MIB2_STATS_NETIF_INC(netif, ifindiscards);
    return NULL;
  }

  p = pbuf_alloc(PBUF_RAW, frame_length, PBUF_POOL);
  if (p == NULL) {
    Gmac_Ip_ProvideRxBuff(GMAC_INSTANCE, GMAC_RING, &state->rx_buffer);
    LINK_STATS_INC(link.memerr);
    LINK_STATS_INC(link.drop);
    MIB2_STATS_NETIF_INC(netif, ifindiscards);
    return NULL;
  }

  for (q = p; q != NULL; q = q->next) {
    MEMCPY(q->payload, &state->rx_buffer.Data[copied], q->len);
    copied = (u16_t)(copied + q->len);
  }
  Gmac_Ip_ProvideRxBuff(GMAC_INSTANCE, GMAC_RING, &state->rx_buffer);

  MIB2_STATS_NETIF_ADD(netif, ifinoctets, frame_length);
  if (((u8_t *)p->payload)[0] & 1U) {
    MIB2_STATS_NETIF_INC(netif, ifinnucastpkts);
  } else {
    MIB2_STATS_NETIF_INC(netif, ifinucastpkts);
  }
  LINK_STATS_INC(link.recv);
  return p;
}

static void low_level_init(struct netif *netif)
{
  struct ethernetif *state = (struct ethernetif *)netif->state;

  Gmac_Ip_GetMacAddr(GMAC_INSTANCE, state->mac);
  netif->hwaddr_len = ETH_HWADDR_LEN;
  MEMCPY(netif->hwaddr, state->mac, ETH_HWADDR_LEN);
  netif->mtu = 1500U;
  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP |
                NETIF_FLAG_LINK_UP;
  state->rx_buffer.Data = NULL_PTR;
  state->rx_buffer.Length = GMAC_FRAME_BUFFER_SIZE;
}

void ethernetif_input(struct netif *netif)
{
  struct pbuf *p;
  u16_t processed = 0U;

  reclaim_tx_buffers(netif);
  while (processed < GMAC_RX_BATCH_SIZE) {
    p = low_level_input(netif);
    if (p == NULL) {
      break;
    }
    if (netif->input(p, netif) != ERR_OK) {
      pbuf_free(p);
    }
    processed++;
  }
}

void ethernetif_update_config(struct netif *netif)
{
  LWIP_UNUSED_ARG(netif);
}

void ethernetif_notify_conn_changed(struct netif *netif)
{
  LWIP_UNUSED_ARG(netif);
}

void ethernetif_update_link(struct netif *netif)
{
  uint16 phy_status;

  if (Gmac_Ip_MDIORead(GMAC_INSTANCE, 0U, GMAC_PHY_BMSR,
                       &phy_status, 10U) == GMAC_STATUS_SUCCESS) {
    if ((phy_status & GMAC_PHY_LINK_STATUS) != 0U) {
      netif_set_link_up(netif);
    } else {
      netif_set_link_down(netif);
    }
  }
}

err_t ethernetif_init(struct netif *netif)
{
  LWIP_ASSERT("netif != NULL", netif != NULL);
  netif->state = &ethernetif_state;
  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;
  netif->output = etharp_output;
  netif->linkoutput = low_level_output;
#if LWIP_IPV6
  netif->output_ip6 = ethip6_output;
#endif
  MIB2_INIT_NETIF(netif, snmp_ifType_ethernet_csmacd, 100000000U);
  low_level_init(netif);
  return ERR_OK;
}