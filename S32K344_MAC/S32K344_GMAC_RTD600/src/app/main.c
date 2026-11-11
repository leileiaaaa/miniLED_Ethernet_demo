#include "Mcal.h"
#include "Gmac_Ip.h"
#include "Siul2_Port_Ip.h"
#include "Clock_Ip.h"
#include "OsIf.h"

#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/timeouts.h"
#include "lwip/ip4_addr.h"
#include "netif/ethernet.h"
#include "ethernetif.h"
#include "echo/echo.h"

#define INST_GMAC_0    (0U)

static struct netif gnetif;

/* Convert the RTD system timer ticks to the millisecond unit required by lwIP. */
u32_t sys_now(void)
{
    static uint32 last_ticks;
    static uint32 elapsed_ticks;
    static u32_t elapsed_ms;
    static boolean initialized = FALSE;
    uint32 delta;
    uint32 ticks_per_ms;

    if (initialized == FALSE)
    {
        last_ticks = OsIf_GetCounter(OSIF_COUNTER_SYSTEM);
        initialized = TRUE;
        return elapsed_ms;
    }

    delta = OsIf_GetElapsed(&last_ticks, OSIF_COUNTER_SYSTEM);
    ticks_per_ms = OsIf_MicrosToTicks(1000U, OSIF_COUNTER_SYSTEM);
    if (ticks_per_ms == 0U)
    {
        ticks_per_ms = 1U;
    }
    elapsed_ticks += delta;
    elapsed_ms += (u32_t)(elapsed_ticks / ticks_per_ms);
    elapsed_ticks %= ticks_per_ms;
    return elapsed_ms;
}

int main(void)
{
    Gmac_Ip_StatusType init_status;
    Clock_Ip_StatusType clock_status;
    ip4_addr_t ipaddr;
    ip4_addr_t netmask;
    ip4_addr_t gateway;

    OsIf_Init(NULL_PTR);
    Siul2_Port_Ip_Init(NUM_OF_CONFIGURED_PINS_PortContainer_0_VS_0,
                       g_pin_mux_InitConfigArr_PortContainer_0_VS_0);

    clock_status = Clock_Ip_Init(&Clock_Ip_aClockConfig[0U]);
    if (clock_status != CLOCK_IP_SUCCESS)
    {
        while (1) { }
    }

    init_status = Gmac_Ip_Init(INST_GMAC_0, &Gmac_0_ConfigPB);
    if (init_status != GMAC_STATUS_SUCCESS)
    {
        while (1) { }
    }
    Gmac_Ip_EnableMDIO(INST_GMAC_0, 0U, 100000000UL);

    IP4_ADDR(&ipaddr, 192U, 168U, 2U, 10U);
    IP4_ADDR(&netmask, 255U, 255U, 255U, 0U);
    IP4_ADDR(&gateway, 192U, 168U, 2U, 1U);

    lwip_init();
    if (netif_add(&gnetif, &ipaddr, &netmask, &gateway, NULL_PTR,
                  ethernetif_init, ethernet_input) == NULL)
    {
        while (1) { }
    }

    netif_set_default(&gnetif);
    netif_set_up(&gnetif);
    ethernetif_update_link(&gnetif);

    echo_init();

    uint32 loop_counter = 0;

    while (1)
    {
        ethernetif_input(&gnetif);

        loop_counter++;
        if ((loop_counter & 0x3FFU) == 0U)
        {
            ethernetif_update_link(&gnetif);
        }

        sys_check_timeouts();
    }
}