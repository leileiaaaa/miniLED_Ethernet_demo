/*==================================================================================================
*   Project              : RTD AUTOSAR 4.7 
*   Platform             : CORTEXM
*   Peripheral           : SIUL2
*   Dependencies         : none
*
*   Autosar Version      : 4.7.0
*   Autosar Revision     : ASR_REL_4_7_REV_0000
*   Autosar Conf.Variant :
*   SW Version           : 6.0.0
*   Build Version        : S32K3_S32M27x_Real-Time_Drivers_AUTOSAR_R21-11_Version_6_0_0_QLP04_HF01_D2510_ASR_REL_4_7_REV_0000_20251010
*
*   Copyright 2020 - 2025 NXP
*
*   NXP Confidential. This software is owned or controlled by NXP and may only be
*   used strictly in accordance with the applicable license terms. By expressly
*   accepting such terms or by downloading, installing, activating and/or otherwise
*   using the software, you are agreeing that you have read, and that you agree to
*   comply with and are bound by, such license terms. If you do not agree to be
*   bound by the applicable license terms, then you may not retain, install,
*   activate or otherwise use the software.
==================================================================================================*/

/**
*   @file      Tspc_Port_Ip_Cfg.h
*
*   @addtogroup Port_CFG
*   @{
*/

#ifdef __cplusplus
extern "C"{
#endif


/*==================================================================================================
                                         INCLUDE FILES
 1) system and project includes
 2) needed interfaces from external units
 3) internal and external interfaces from this unit
==================================================================================================*/
#include "Tspc_Port_Ip_Cfg.h"

/*==================================================================================================
*                              SOURCE FILE VERSION INFORMATION
==================================================================================================*/
#define TSPC_PORT_IP_VENDOR_ID_CFG_C                       43
#define TSPC_PORT_IP_AR_RELEASE_MAJOR_VERSION_CFG_C        4
#define TSPC_PORT_IP_AR_RELEASE_MINOR_VERSION_CFG_C        7
#define TSPC_PORT_IP_AR_RELEASE_REVISION_VERSION_CFG_C     0
#define TSPC_PORT_IP_SW_MAJOR_VERSION_CFG_C                6
#define TSPC_PORT_IP_SW_MINOR_VERSION_CFG_C                0
#define TSPC_PORT_IP_SW_PATCH_VERSION_CFG_C                0

/*==================================================================================================
*                                     FILE VERSION CHECKS
==================================================================================================*/
/* Check if Tspc_Port_Ip_Cfg.c and Tspc_Port_Ip_Cfg.h are of the same vendor */
#if (TSPC_PORT_IP_VENDOR_ID_CFG_C != TSPC_PORT_IP_VENDOR_ID_CFG_H)
    #error "Tspc_Port_Ip_Cfg.c and Tspc_Port_Ip_Cfg.h have different vendor ids"
#endif
/* Check if Tspc_Port_Ip_Cfg.c and Tspc_Port_Ip_Cfg.h are of the same Autosar version */
#if ((TSPC_PORT_IP_AR_RELEASE_MAJOR_VERSION_CFG_C    != TSPC_PORT_IP_AR_RELEASE_MAJOR_VERSION_CFG_H) || \
    (TSPC_PORT_IP_AR_RELEASE_MINOR_VERSION_CFG_C    != TSPC_PORT_IP_AR_RELEASE_MINOR_VERSION_CFG_H) || \
    (TSPC_PORT_IP_AR_RELEASE_REVISION_VERSION_CFG_C != TSPC_PORT_IP_AR_RELEASE_REVISION_VERSION_CFG_H) \
    )
    #error "AutoSar Version Numbers of Tspc_Port_Ip_Cfg.c and Tspc_Port_Ip_Cfg.h are different"
#endif
/* Check if Tspc_Port_Ip_Cfg.c and Tspc_Port_Ip_Cfg.h are of the same Software version */
#if ((TSPC_PORT_IP_SW_MAJOR_VERSION_CFG_C != TSPC_PORT_IP_SW_MAJOR_VERSION_CFG_H) || \
    (TSPC_PORT_IP_SW_MINOR_VERSION_CFG_C != TSPC_PORT_IP_SW_MINOR_VERSION_CFG_H) || \
    (TSPC_PORT_IP_SW_PATCH_VERSION_CFG_C != TSPC_PORT_IP_SW_PATCH_VERSION_CFG_H)    \
    )
    #error "Software Version Numbers of Tspc_Port_Ip_Cfg.c and Tspc_Port_Ip_Cfg.h are different"
#endif

/*==================================================================================================
                             LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
                                             LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
                                            LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                           LOCAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                           GLOBAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                           GLOBAL VARIABLES
==================================================================================================*/

/* clang-format off */

/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
PortContainer_0_VS_0:
- options: {callFromInitBoot: 'false', coreID: M7_0}
- pin_list:
  - {pin_num: N3, peripheral: EMAC, signal: emac_mii_rmii_mdc, pin_signal: PTD17}
  - {pin_num: P2, peripheral: EMAC, signal: emac_mii_rmii_mdio, pin_signal: PTD16, direction: INPUT/OUTPUT}
  - {pin_num: U8, peripheral: EMAC, signal: 'emac_mii_rmii_rxd, 0', pin_signal: PTD9}
  - {pin_num: R8, peripheral: EMAC, signal: 'emac_mii_rmii_rxd, 1', pin_signal: PTD8}
  - {pin_num: U2, peripheral: EMAC, signal: 'emac_mii_rmii_txd, 0', pin_signal: PTB5}
  - {pin_num: U3, peripheral: EMAC, signal: 'emac_mii_rmii_txd, 1', pin_signal: PTB4}
  - {pin_num: T8, peripheral: EMAC, signal: emac_mii_rx_clk, pin_signal: PTC1}
  - {pin_num: R6, peripheral: EMAC, signal: emac_mii_rmii_rx_dv, pin_signal: PTC17}
  - {pin_num: P6, peripheral: EMAC, signal: emac_mii_rmii_rx_er, pin_signal: PTC16}
  - {pin_num: U9, peripheral: EMAC, signal: emac_mii_rxd2, pin_signal: PTC15}
  - {pin_num: T9, peripheral: EMAC, signal: emac_mii_rxd3, pin_signal: PTC14}
  - {pin_num: U7, peripheral: EMAC, signal: emac_mii_rmii_tx_clk, pin_signal: PTC0}
  - {pin_num: R3, peripheral: EMAC, signal: emac_mii_rmii_tx_en, pin_signal: PTE9}
  - {pin_num: T5, peripheral: EMAC, signal: emac_mii_txd2, pin_signal: PTD6}
  - {pin_num: U5, peripheral: EMAC, signal: emac_mii_txd3, pin_signal: PTD5}
  - {pin_num: N17, peripheral: SIUL2, signal: 'gpio, 94', pin_signal: PTC30, direction: OUTPUT, InitValue: state_1}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

/* No registers that support TSPC were configured*/

/*==================================================================================================
                                      LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

/*==================================================================================================
                                           LOCAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================
                                           GLOBAL FUNCTIONS
==================================================================================================*/


#ifdef __cplusplus
}
#endif

/** @} */
