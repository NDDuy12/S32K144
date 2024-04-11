#include "S32K144.h"
#include "CAN.h"
#include "Timer.h"
#define MSG_BUF_SIZE 4 /* Msg Buffer Size. (CAN 2.0AB: 2 hdr + 2 data= 4 words) */
Rx REC;
//-------------------------------------------------------------------------------------------------------------
// CAN Init
//-------------------------------------------------------------------------------------------------------------
void FLEXCAN0_init(void) {

	uint32_t i=0;
	PCC->PCCn[PCC_FlexCAN0_INDEX] |= PCC_PCCn_CGC_MASK; /* CGC=1: enable clock to FlexCAN0 */
	CAN0->MCR |= CAN_MCR_MDIS_MASK; /* MDIS=1: Disable module before selecting clock */ //0x80000000
	CAN0->CTRL1 &= ~CAN_CTRL1_CLKSRC_MASK; /* CLKSRC=0: Clock Source = oscillator (8 MHz) */ //0x2000
	CAN0->MCR &= ~CAN_MCR_MDIS_MASK; /* MDIS=0; Enable module config. (Sets FRZ, HALT)*/
	while (!((CAN0->MCR & CAN_MCR_FRZACK_MASK) >> CAN_MCR_FRZACK_SHIFT)) {}
		/* Good practice: wait for FRZACK=1 on freeze mode entry/exit */
	CAN0->CTRL1 = 0x00DB0006; /* Configure for 500 KHz bit time */
		/* Time quanta freq = 16 time quanta x 500 KHz bit time= 8MHz */
		/* PRESDIV+1 = Fclksrc/Ftq = 8 MHz/8 MHz = 1 */
		/* so PRESDIV = 0 */
		/* PSEG2 = Phase_Seg2 - 1 = 4 - 1 = 3 */
		/* PSEG1 = PSEG2 = 3 */
		/* PROPSEG= Prop_Seg - 1 = 7 - 1 = 6 */
		/* RJW: since Phase_Seg2 >=4, RJW+1=4 so RJW=3. */
		/* SMP = 1: use 3 bits per CAN sample */
		/* CLKSRC=0 (unchanged): Fcanclk= Fosc= 8 MHz */
	for(i=0; i<128; i++ ) { /* CAN0: clear 32 msg bufs x 4 words/msg buf = 128 words*/
		CAN0->RAMn[i] = 0; /* Clear msg buf word */
	}
	for(i=0; i<16; i++ ) { /* In FRZ mode, init CAN0 16 msg buf filters */
		CAN0->RXIMR[i] = 0xFFFFFFFF; /* Check all ID bits for incoming messages */
	}
	CAN0->RXMGMASK = 0x1FFFFFFF; /* Global acceptance mask: check all ID bits */
	CAN0->RAMn[ 4*MSG_BUF_SIZE + 0] = 0x04000000; /* Msg Buf 4, word 0: Enable for reception */
	/* EDL,BRS,ESI=0: CANFD not used */
	/* CODE=4: MB set to RX inactive */
	/* IDE=0: Standard ID */
	/* SRR, RTR, TIME STAMP = 0: not applicable */
#if (NODE_A) /* Node A receives msg with std ID 0x0A0*/
	CAN0->RAMn[ 4*MSG_BUF_SIZE + 1] = 0x02C00000; /* Msg Buf 4, word 1: Standard ID = 0x0A0 */
#else /* Node B to receive msg with std ID 0x0B0 */
	CAN0->RAMn[ 4*MSG_BUF_SIZE + 1] = 0x02800000; /* Msg Buf 4, word 1: Standard ID = 0x0B0 */
#endif
				/* PRIO = 0: CANFD not used */
	CAN0->MCR = 0x0000001F; /* Negate FlexCAN 1 halt state for 32 MBs */
	while ((CAN0->MCR && CAN_MCR_FRZACK_MASK) >> CAN_MCR_FRZACK_SHIFT) {}
	/* Good practice: wait for FRZACK to clear (not in freeze mode) */
	while ((CAN0->MCR && CAN_MCR_NOTRDY_MASK) >> CAN_MCR_NOTRDY_SHIFT) {}
	/* Good practice: wait for NOTRDY to clear (module ready) */
}
//-------------------------------------------------------------------------------------------------------------
// CAN TRANSMIT
//-------------------------------------------------------------------------------------------------------------
void FLEXCAN0_transmit_msg(uint32_t data1,uint32_t data2) { /* Assumption: Message buffer CODE is INACTIVE */
	CAN0->IFLAG1 = 0x00000001; /* Clear CAN 0 MB 0 flag without clearing others*/
	CAN0->RAMn[ 0*MSG_BUF_SIZE + 2] = data1; /* MB0 word 2: data word 0 */
	CAN0->RAMn[ 0*MSG_BUF_SIZE + 3] = data2; /* MB0 word 3: data word 1 */
#if (NODE_A)
	CAN0->RAMn[ 0*MSG_BUF_SIZE + 1] = 0x02800000; /* MB0 word 1: Tx msg with STD ID 0x0B0 */
	
#else	
 CAN0->RAMn[ 0*MSG_BUF_SIZE + 1] = 0x02C00000; /* MB0 word 1: Tx msg with STD ID 0x0A0 */
#endif
	 CAN0->RAMn[ 0*MSG_BUF_SIZE + 0] = 0x0C400000 | 8 <<CAN_WMBn_CS_DLC_SHIFT; /* MB0 word 0: */
	/* EDL,BRS,ESI=0: CANFD not used */
	/* CODE=0xC: Activate msg buf to transmit */
	/* IDE=0: Standard ID */
	/* SRR=1 Tx frame (not req'd for std ID) */
	/* RTR = 0: data, not remote tx request frame*/
	/* DLC = 8 bytes */
}
//-------------------------------------------------------------------------------------------------------------
// CAN RECEIVE
//-------------------------------------------------------------------------------------------------------------
void FLEXCAN0_receive_msg(void) { /* Receive msg from ID 0x556 using msg buffer 4 */
	 uint8_t j;
	 uint32_t dummy;
	 REC.CODE = (CAN0->RAMn[ 4*MSG_BUF_SIZE + 0] & 0x07000000) >> 24; /* Read CODE field */
	 REC.ID = (CAN0->RAMn[ 4*MSG_BUF_SIZE + 1] & CAN_WMBn_ID_ID_MASK) >> CAN_WMBn_ID_ID_SHIFT;
	 REC.LENGTH =(CAN0->RAMn[ 4*MSG_BUF_SIZE + 0] & CAN_WMBn_CS_DLC_MASK)>>CAN_WMBn_CS_DLC_SHIFT;
	 for (j=0; j<2; j++) { /* Read two words of data (8 bytes) */
	 REC.DATA[j] = CAN0->RAMn[ 4*MSG_BUF_SIZE + 2 + j];
	 }
	 REC.TIMESTAMP = (CAN0->RAMn[ 0*MSG_BUF_SIZE + 0] & 0x000FFFF);
	 dummy = CAN0->TIMER; /* Read TIMER to unlock message buffers */
	 CAN0->IFLAG1 = 0x00000010; /* Clear CAN 0 MB 4 flag without clearing others*/
}