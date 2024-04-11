#include "S32K144.h"
#include "GPIO.h"
//-------------------------------------------------------------------------------------------------------------
// Port Init
//-------------------------------------------------------------------------------------------------------------
void PORT_init(void) {
	PCC->PCCn[PCC_PORTD_INDEX ]|=PCC_PCCn_CGC_MASK; /* Enable clock for PORTD */
	PCC->PCCn[PCC_PORTE_INDEX ]|=PCC_PCCn_CGC_MASK; /* Enable clock for PORTE */
	PCC->PCCn[PCC_PORTC_INDEX ]|=PCC_PCCn_CGC_MASK; /* Enable clock for PORTC */
	 /* CAN Port */
	PORTE->PCR[4] |= PORT_PCR_MUX(5); /* Port E4: MUX = ALT5, CAN0_RX */
	PORTE->PCR[5] |= PORT_PCR_MUX(5); /* Port E5: MUX = ALT5, CAN0_TX */
	 /* GPIO Port */
	PORTE->PCR[8]|=PORT_PCR_MUX(2); /* Port E8: MUX = ALT2, FTM0CH6 || Input Capture*/
	PORTD->PCR[15]|=PORT_PCR_MUX(2); /* Port D15: MUX = ALT2, FTM0CH0 || LED RED PWM */
	PORTD->PCR[0]|=PORT_PCR_MUX(2); /* Port D15: MUX = ALT2, FTM0CH2 || LED BLUE PWM */
	PORTD->PCR[16]|=PORT_PCR_MUX(1); /* Port D16: MUX = GPIO (to green LED)|| Signal CAN LED GREEN */
	PORTC -> PCR[12] |= (1<<8); /* Port C12: MUX = GPIO Switch2 */
	PORTC -> PCR[13] |= 1<<8; /* Port C12: MUX = GPIO Switch3 */
	
	PTC -> PDDR |= ~(1<<12); /* Port C12: Data Direction= input (default) */
	PTC -> PDDR |= ~(1<<13); /* Port C12: Data Direction= input (default) */
	//	PORTD->PCR[16] = 0x00000100; /* Port D16: MUX = GPIO (to green LED)|| Signal CAN LED GREEN */
	//	PORTD->PCR[16]|=PORT_PCR_MUX(2); /* Port D16: MUX = ALT2, FTM0CH1 */
}
