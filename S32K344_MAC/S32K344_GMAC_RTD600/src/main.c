#ifdef __cplusplus
extern "C" {
#endif


/*==================================================================================================
*                                        INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
==================================================================================================*/
#include "Mcal.h"
#include "Gmac_Ip.h"
#include "Siul2_Port_Ip.h"
#include "Clock_Ip.h"
#include "OsIf.h"
#include "Siul2_Dio_Ip.h"

#include "udp.h"


/*==================================================================================================
*                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/* 以太网帧结构体 */
typedef struct
{
	uint8_t destMacaddress[6];
	uint8_t srcMacaddress[6];
	uint16_t length;
	uint8_t data[1500];
}mac_frame;

/*==================================================================================================
*                                       LOCAL MACROS
==================================================================================================*/
#define INST_GMAC_0    (0U)
/*==================================================================================================
*                                      LOCAL CONSTANTS
*==================================================================================================*/

/* 板卡自己的 IP（按需修改，和 udp.h BOARD_IP 保持一致） */
static const uint8 BOARD_IP_ADDR[4] = { 192U, 168U, 2U, 10U };

/*==================================================================================================
*                                      LOCAL VARIABLES
==================================================================================================*/


/*==================================================================================================
*                                      GLOBAL CONSTANTS
==================================================================================================*/
#define UDP_T     0
#define Ethernet  1
#define SEND      1
#define RECV      1
/*==================================================================================================
*                                      GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/


/*==================================================================================================
*                                       LOCAL FUNCTIONS
==================================================================================================*/
void copy_buffer(uint8_t *dest, uint8_t *src, uint8_t len)
{
	uint8_t i;

	for (i = 0; i < len; i++)
	{
		dest[i] = src[i];
	}
}
/*==================================================================================================
*                                       GLOBAL FUNCTIONS
==================================================================================================*/
/**
* @brief        Transmit & receive in internal loopback mode
* @details
*/
// int main(void)
// {
//     Gmac_Ip_TxOptionsType TxOptions = {TRUE, GMAC_CRC_AND_PAD_INSERTION, GMAC_CHECKSUM_INSERTION_DISABLE};
//     Gmac_Ip_BufferType TxBuffer = {0};
//     Gmac_Ip_TxInfoType TxInfo  = {0};
//     Gmac_Ip_BufferType RxBuffer = {0};
//     Gmac_Ip_RxInfoType RxInfo  = {0};
//     boolean Result = TRUE;
//     Gmac_Ip_StatusType Status;
//     uint8 MacAddr[6U] = {0U};
//     uint8 DstAddr[6] = {0x22,0x33,0x44,0x55,0x66,0x99};
//     uint8 i;
//     volatile uint8 tmp[64] = {0};

// 	uint16_t reg0;
// 	uint16_t reg1;
// 	uint16_t reg17;
// 	uint16_t reg18;
// 	uint16_t reg19;
// 	uint16_t reg21;
// 	uint16_t reg20;
// 	uint16_t reg23;
// 	uint16_t reg25;
// 	uint16_t reg24;
// 	uint16_t reg26;

// 	volatile Gmac_Ip_StatusType MdioStatus[11];

// #if UDP_T
//     uint8_t data_tx[8] = {0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68};
// #endif


//     /* 系统初始化 */
//     OsIf_Init(NULL_PTR);

//     /* GPIO初始化 */
//     Siul2_Port_Ip_Init(NUM_OF_CONFIGURED_PINS_PortContainer_0_VS_0, g_pin_mux_InitConfigArr_PortContainer_0_VS_0);

//     volatile uint32 timeout = 0xFFFF;
//     while(timeout--){}
//     /* 时钟初始化 */
//     Clock_Ip_Init(&Clock_Ip_aClockConfig[0U]);

//     /* GMAC初始化 */
//     Gmac_Ip_Init(INST_GMAC_0, &Gmac_0_ConfigPB);

//     /* 使能SMI 读写PHY寄存器*/
//     Gmac_Ip_EnableMDIO(0, 0, 100000000UL);

//     /* 得到主机MAC地址 */
//     Gmac_Ip_GetMacAddr(INST_GMAC_0, MacAddr);

// 	Gmac_Ip_MDIOWrite(0,0,17,0x9804,1000);

// 	Gmac_Ip_MDIOWrite(0,0,18,0xDC01,1000);/*主机*/
// 	Gmac_Ip_MDIOWrite(0,0,0,0x6100,1000);

// 	Gmac_Ip_MDIOWrite(0,0,17,0x9800,1000);

//     /* 申请一个至少64字节的Buffer */
//     TxBuffer.Length = 64U;
//     /* 关联TxBuff和MDA通道 */
//     if ((GMAC_STATUS_SUCCESS != Gmac_Ip_GetTxBuff(INST_GMAC_0, 0U, &TxBuffer, NULL_PTR)) || (TxBuffer.Length < 64U))
//     {
//         Result = FALSE;
//     }

//     /* 把不同的帧放入TxBuffer */
// #if Ethernet

//     /* 做一个以太网帧 */
//     mac_frame frame;
//     for (i = 0U; i < 6U; i++)
//     {
//     	frame.srcMacaddress[i] = MacAddr[i];
//     	frame.destMacaddress[i] = DstAddr[i];
//     }
//     frame.length = 60;

//     /* 数据长度 = Frame - (DstAddr + SrcAddr + EtherType/Length + FCS) 64U - (6U + 6U + 2U + 4U)*/
//     for(i = 0; i < 46; i++)
//       {
//       	frame.data[i] = i;
//       }

//     /* 把帧放入Buffer */
//     copy_buffer(*(&TxBuffer.Data),(uint8_t *)&frame,60);
//     for(i = 0; i < 64; i++)
//     {
//     	tmp[i] = *((uint8 *)(&TxBuffer.Data[0U] + i));
//     }
// #endif
// 	/* 调试查看PHY是 否异常*/
// 	MdioStatus[0]  = Gmac_Ip_MDIORead(0,0,0,&reg0,1000);  //8448  全字节100M
// 	MdioStatus[1]  = Gmac_Ip_MDIORead(0,0,1,&reg1,1000);  //485 link_up 自动协商完成
// 	MdioStatus[2]  = Gmac_Ip_MDIORead(0,0,17,&reg17,1000);//38912  接上且正常模式
// 	MdioStatus[3]  = Gmac_Ip_MDIORead(0,0,18,&reg18,1000);//56325  56321
// 	MdioStatus[4]  = Gmac_Ip_MDIORead(0,0,19,&reg19,1000);//4677   5189
// 	MdioStatus[5]  = Gmac_Ip_MDIORead(0,0,21,&reg21,1000);//
// 	MdioStatus[6]  = Gmac_Ip_MDIORead(0,0,20,&reg20,1000);//
// 	MdioStatus[7]  = Gmac_Ip_MDIORead(0,0,23,&reg23,1000);//48356
// 	MdioStatus[8]  = Gmac_Ip_MDIORead(0,0,24,&reg24,1000);//0              16384
// 	MdioStatus[9]  = Gmac_Ip_MDIORead(0,0,25,&reg25,1000);//0
// 	MdioStatus[10] = Gmac_Ip_MDIORead(0,0,26,&reg26,1000);//0

// #if UDP_T
//    /* 做一个UDP 包*/
//     buid_udp_packet(TxBuffer.Data, data_tx, 8);
// #endif

//     while(1)
//     {
//     	do
//     	{
//     		Gmac_Ip_MDIORead(0,0,1,&reg1,1000); //485 link_up 自动协商完成
//     	}while((reg1 & 0x4) != 0x4);
// #if SEND
//     	/* 发送数据 */
// 		TxBuffer.Length = 64U - 4U;     /* 不计算FCS，因为这4字节会被控制器自动插入 */
// 		if (GMAC_STATUS_SUCCESS != Gmac_Ip_SendFrame(INST_GMAC_0, 0U, &TxBuffer, &TxOptions))
// 		{
// 			Result = FALSE;
// 		}

// 		/* 等待发送完成*/
// 		do {
// 			Status = Gmac_Ip_GetTransmitStatus(INST_GMAC_0, 0U, &TxBuffer, &TxInfo);
// 		} while (Status == GMAC_STATUS_BUSY);

// 		/* 判断发送状态  */
// 		if ((GMAC_STATUS_SUCCESS != Status) || (0U != TxInfo.ErrMask))
// 		{
// 			Result = FALSE;
// 		}
// #endif


// #if RECV
// 		/* 接收数据 */
// 		do{
// 			Status = Gmac_Ip_ReadFrame(INST_GMAC_0, 0U, &RxBuffer, &RxInfo);
// 		}while(Status == GMAC_STATUS_RX_QUEUE_EMPTY);
// 		/* 提供一个接收数据的buffer */
// 		 Gmac_Ip_ProvideRxBuff(INST_GMAC_0, 0U, &RxBuffer);
// 		 /* debug 查看接收数据*/
// 	    for (i = 0U; i < 60; i++)
// 		{
// 			if( tmp[i] != *((uint8 *)(&RxBuffer.Data[0U] + i)))
// 			{
// 				Result = FALSE;
// 				break;
// 			}

// 		}

// #endif
// 	    if(Result == TRUE)
// 	    {
// 	    	/* 成功发送了 数据 */
// 	    	Siul2_Dio_Ip_ClearPins(LED4_PORT,(1 << LED4_PIN));
// 	    	;
// 	    }
// 		/* 调试查看PHY是 否异常*/
// 		Gmac_Ip_MDIORead(0,0,0,&reg0,1000); //8448  全字节100M
// 		Gmac_Ip_MDIORead(0,0,1,&reg1,1000); //485 link_up 自动协商完成
// 		Gmac_Ip_MDIORead(0,0,17,&reg17,1000);//38912  接上且正常模式
// 		Gmac_Ip_MDIORead(0,0,18,&reg18,1000);//56325  56321
// 		Gmac_Ip_MDIORead(0,0,19,&reg19,1000);//4677   5189
// 		Gmac_Ip_MDIORead(0,0,21,&reg21,1000);//
// 		Gmac_Ip_MDIORead(0,0,20,&reg20,1000);//
// 		Gmac_Ip_MDIORead(0,0,23,&reg23,1000);//48356
// 		Gmac_Ip_MDIORead(0,0,24,&reg24,1000);//0              16384
// 		Gmac_Ip_MDIORead(0,0,25,&reg25,1000);//0
// 		Gmac_Ip_MDIORead(0,0,26,&reg26,1000);//0
//     }
//     /* 失能控制器 */
//     Gmac_Ip_DisableController(INST_GMAC_0);ip.dst == 192.168.2.100

//     return 0;
// }

int main(void)
{
    Gmac_Ip_TxOptionsType TxOptions = {TRUE, GMAC_CRC_AND_PAD_INSERTION, GMAC_CHECKSUM_INSERTION_DISABLE};
    Gmac_Ip_BufferType TxBuffer = {NULL_PTR, 1536U};
    Gmac_Ip_BufferType RxBuffer = {NULL_PTR, 1536U};
    Gmac_Ip_TxInfoType TxInfo;
    Gmac_Ip_RxInfoType RxInfo;

    volatile Gmac_Ip_StatusType InitStatus;
    volatile Gmac_Ip_StatusType TxStatus;
    volatile Gmac_Ip_StatusType TxDoneStatus;
    volatile Gmac_Ip_StatusType RxStatus;
    volatile Clock_Ip_StatusType ClockStatus;
    volatile Gmac_Ip_StatusType MdioStatus[3];

    uint8 MacAddr[6U] = {0U, 0U, 0U, 0U, 0U, 0U};
    uint8 DstAddr[6U] = {0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU};
    uint16 PhyId1 = 0U;
    uint16 PhyId2 = 0U;
    uint16 Bmsr = 0U;
    uint8 i;
    volatile uint32 Delay;

    OsIf_Init(NULL_PTR);

    Siul2_Port_Ip_Init(NUM_OF_CONFIGURED_PINS_PortContainer_0_VS_0,
                       g_pin_mux_InitConfigArr_PortContainer_0_VS_0);

    ClockStatus = Clock_Ip_Init(&Clock_Ip_aClockConfig[0U]);
    if (CLOCK_IP_SUCCESS != ClockStatus)
    {
        while (1)
        {
        }
    }

    InitStatus = Gmac_Ip_Init(INST_GMAC_0, &Gmac_0_ConfigPB);
    if (GMAC_STATUS_SUCCESS != InitStatus)
    {
        while (1)
        {
        }
    }

    Gmac_Ip_EnableMDIO(INST_GMAC_0, 0U, 100000000UL);

    MdioStatus[0] = Gmac_Ip_MDIORead(INST_GMAC_0, 0U, 2U, &PhyId1, 10U);
    MdioStatus[1] = Gmac_Ip_MDIORead(INST_GMAC_0, 0U, 3U, &PhyId2, 10U);
    MdioStatus[2] = Gmac_Ip_MDIORead(INST_GMAC_0, 0U, 1U, &Bmsr, 10U);

    Gmac_Ip_GetMacAddr(INST_GMAC_0, MacAddr);

    while (1)
    {
        /* =============================================
         * 1. 接收 —— 非阻塞，队列空就下一轮
         * ============================================= */
        RxStatus = Gmac_Ip_ReadFrame(INST_GMAC_0, 0U, &RxBuffer, &RxInfo);
        if (GMAC_STATUS_SUCCESS != RxStatus)
        {
            continue;
        }

        /* PktLen 含 FCS 4 字节；不含 FCS 的有效帧长度 */
        {
            uint16_t frame_len = (RxInfo.PktLen > 4U) ? (RxInfo.PktLen - 4U) : 0U;
            if (frame_len < 42U)   /* 至少 14 eth + 20 ip + 8 icmp 头，或 14 eth + 28 arp */
            {
                Gmac_Ip_ProvideRxBuff(INST_GMAC_0, 0U, &RxBuffer);
                continue;
            }

            /* =============================================
             * 2. EtherType 判断 —— 大端 16-bit
             * ============================================= */
            {
                uint16_t ethtype = ((uint16_t)RxBuffer.Data[FRAME_ETH_TYPE_OFFSET] << 8)
                                 |  (uint16_t)RxBuffer.Data[FRAME_ETH_TYPE_OFFSET + 1];

                if (ethtype == ETHERTYPE_ARP)
                {
                    /* ==================== ARP ====================
                     * ARP Request → 原地改造成 ARP Reply
                     * ARP 帧结构（以太网 ARP，42 字节）：
                     *   [0..5]  eth dest MAC  
                     *   [6..11] eth src MAC
                     *   [12..13] EtherType = 0x0806
                     *   [14..15] HTYPE = 0x0001 (Ethernet)
                     *   [16..17] PTYPE = 0x0800 (IPv4)
                     *   [18]     HLEN = 6
                     *   [19]     PLEN = 4
                     *   [20..21] OPER = 1=Request, 2=Reply
                     *   [22..27] Sender MAC
                     *   [28..31] Sender IP
                     *   [32..37] Target MAC (Request 时全 0)
                     *   [38..41] Target IP
                     * ============================================= */
                    if (frame_len >= 42U)
                    {
                        uint16_t arp_op = ((uint16_t)RxBuffer.Data[20] << 8)
                                        |  (uint16_t)RxBuffer.Data[21];
                        if (arp_op == 1U)   /* ARP Request */
                        {
                            /* 翻转以太网头：dest ← 原 src；src ← 板卡 MAC */
                            copy_buff(&RxBuffer.Data[0], &RxBuffer.Data[6], 6);
                            copy_buff(&RxBuffer.Data[6], (uint8_t *)MacAddr, 6);

                            /* OPER: 1 → 2 (Reply) */
                            RxBuffer.Data[20] = 0;
                            RxBuffer.Data[21] = 2;

                            /* Sender ↔ Target 交换 */
                            copy_buff(&RxBuffer.Data[32], &RxBuffer.Data[22], 10); /* target MAC ← sender MAC */
                            copy_buff(&RxBuffer.Data[22], (uint8_t *)MacAddr, 6);   /* sender MAC ← board MAC */
                            copy_buff(&RxBuffer.Data[38], &RxBuffer.Data[28], 4);  /* target IP ← sender IP */
                            copy_buff(&RxBuffer.Data[28], (uint8_t *)BOARD_IP_ADDR, 4); /* sender IP ← board IP */

                            /* 发回去 —— ARP 固定 42 字节 */
                            TxBuffer.Length = 1536U;
                            TxStatus = Gmac_Ip_GetTxBuff(INST_GMAC_0, 0U, &TxBuffer, NULL_PTR);
                            if (GMAC_STATUS_SUCCESS == TxStatus)
                            {
                                for (i = 0U; i < 42U; i++)
                                {
                                    TxBuffer.Data[i] = RxBuffer.Data[i];
                                }
                                TxBuffer.Length = 42U;
                                Gmac_Ip_SendFrame(INST_GMAC_0, 0U, &TxBuffer, &TxOptions);
                                do {
                                    TxDoneStatus = Gmac_Ip_GetTransmitStatus(INST_GMAC_0, 0U, &TxBuffer, &TxInfo);
                                } while (GMAC_STATUS_BUSY == TxDoneStatus);
                            }
                        }
                    }
                    Gmac_Ip_ProvideRxBuff(INST_GMAC_0, 0U, &RxBuffer);
                    continue;
                }

                if (ethtype != ETHERTYPE_IPV4)
                {
                    /* 其他 EtherType（VLAN tagged、802.1x、IPv6...）暂不处理 */
                    Gmac_Ip_ProvideRxBuff(INST_GMAC_0, 0U, &RxBuffer);
                    continue;
                }

                /* =============================================
                 * 3. IPv4 Protocol 判断
                 * ============================================= */
                {
                    uint8 ip_proto = RxBuffer.Data[FRAME_IP_PROTO_OFFSET];   /* 偏移 23 */

                    if ((ip_proto == FRAME_IP_PROTO_ICMP) && (frame_len >= 38U))
                    {
                        /* ---------- ICMP ---------- */
                        uint8 icmp_type = RxBuffer.Data[FRAME_ICMP_TYPE_OFFSET]; /* 偏移 34 */
                        if (icmp_type == FRAME_ICMP_TYPE_ECHO_REQUEST)
                        {
                            /* Echo Request → 原地改造成 Echo Reply */
                            build_ping_reply_ex(RxBuffer.Data, MacAddr, BOARD_IP_ADDR, frame_len);

                            /* 申请 Tx buffer，拷贝修改后的帧发回去 */
                            TxBuffer.Length = 1536U;
                            TxStatus = Gmac_Ip_GetTxBuff(INST_GMAC_0, 0U, &TxBuffer, NULL_PTR);
                            if (GMAC_STATUS_SUCCESS == TxStatus)
                            {
                                for (i = 0U; i < frame_len; i++)
                                {
                                    TxBuffer.Data[i] = RxBuffer.Data[i];
                                }
                                TxBuffer.Length = frame_len;
                                Gmac_Ip_SendFrame(INST_GMAC_0, 0U, &TxBuffer, &TxOptions);
                                do {
                                    TxDoneStatus = Gmac_Ip_GetTransmitStatus(INST_GMAC_0, 0U, &TxBuffer, &TxInfo);
                                } while (GMAC_STATUS_BUSY == TxDoneStatus);
                            }
                        }
                    }
                    else if (ip_proto == 0x11U)   /* UDP = 0x11 */
                    {
                        /* ---------- UDP Echo ---------- */
                        /* 翻转 IP（src↔dest），翻转 UDP 端口，UDP checksum 清 0 */
                        copy_buff(&RxBuffer.Data[FRAME_IP_DEST_OFFSET],
                                  &RxBuffer.Data[FRAME_IP_SRC_OFFSET], 4);
                        copy_buff(&RxBuffer.Data[FRAME_IP_SRC_OFFSET],
                                  (uint8_t *)BOARD_IP_ADDR, 4);

                        /* UDP 头偏移 34：src_port(2) dst_port(2) length(2) checksum(2) */
                        {
                            uint8 t0 = RxBuffer.Data[34];
                            uint8 t1 = RxBuffer.Data[35];
                            RxBuffer.Data[34] = RxBuffer.Data[36];
                            RxBuffer.Data[35] = RxBuffer.Data[37];
                            RxBuffer.Data[36] = t0;
                            RxBuffer.Data[37] = t1;
                            /* UDP checksum 置 0 —— RFC 768 允许 optional */
                            RxBuffer.Data[40] = 0;
                            RxBuffer.Data[41] = 0;
                        }

                        TxBuffer.Length = frame_len;
                        TxStatus = Gmac_Ip_GetTxBuff(INST_GMAC_0, 0U, &TxBuffer, NULL_PTR);
                        if (GMAC_STATUS_SUCCESS == TxStatus)
                        {
                            for (i = 0U; i < frame_len; i++)
                            {
                                TxBuffer.Data[i] = RxBuffer.Data[i];
                            }
                            TxBuffer.Length = frame_len;
                            Gmac_Ip_SendFrame(INST_GMAC_0, 0U, &TxBuffer, &TxOptions);
                            do {
                                TxDoneStatus = Gmac_Ip_GetTransmitStatus(INST_GMAC_0, 0U, &TxBuffer, &TxInfo);
                            } while (GMAC_STATUS_BUSY == TxDoneStatus);
                        }
                    }
                }
            }
        }

        /* =============================================
         * 4. 归还 Rx buffer —— 任何路径都要执行
         * ============================================= */
        Gmac_Ip_ProvideRxBuff(INST_GMAC_0, 0U, &RxBuffer);
    }
}


#ifdef __cplusplus
}
#endif