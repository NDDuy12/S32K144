#ifndef CAN_H_
#define CAN_H_
#include "stdint.h"
typedef struct 
{
	uint32_t CODE; /* Receive	d message buffer code */
	uint32_t ID; /* Received message ID */
	uint32_t LENGTH; /* Received message number of data bytes */
	uint32_t DATA[2]; /* Received message data (2 words) */
	uint32_t *TIMESTAMP; /* Received message time */
} Rx;

void FLEXCAN0_init(void);
void FLEXCAN0_transmit_msg(uint32_t data1,uint32_t data2);
void FLEXCAN0_receive_msg(void);
#endif 