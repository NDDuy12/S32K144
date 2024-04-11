#include "S32K144.h"
#include "GPIO.h"
#include "Clock.h"
#include "Timer.h" 
#include "CAN.h"
#include "ADC.h"
//-------------------------------------------------------------------------------------------------------------
/*define & variables*/
//-------------------------------------------------------------------------------------------------------------
uint32_t data;
extern Rx REC;
uint8_t stateBtn1 =0;
uint8_t enable_transmit=0;//0: disable, 1: enable
//-------------------------------------------------------------------------------------------------------------
/*Function*/
//-------------------------------------------------------------------------------------------------------------
void A_TO_B(void)
{
	if (FTM1->CNT == 62499)
	{
		FLEXCAN0_transmit_msg(read_adc_chx(),0x00000001); /* Transmit message */
	}
	else
	{
	}
}
void B_TO_A(void)
{
	if (FTM1->CNT == 31249)
	{
		FLEXCAN0_transmit_msg(read_adc_chx(),0x00000000); /* Transmit message */
	}
	else
	{
	}
}
//-------------------------------------------------------------------------------------------------------------
/* Main*/
//-------------------------------------------------------------------------------------------------------------
int main(void){
	uint32_t rx_msg_count = 0;
	SOSC_init_8MHz(); /* Init SOSC external 8MHz*/ 
	SPLL_init_80MHz(); /* Init SPLL 80MHz*/
	NormalRUNmode_80MHz(); /*set mode NORMALRUN*/
	PORT_init(); /*GPIO init*/
	PTD->PDDR |= 1<<16; /* Port D16: Data direction = output */
	/*Init Timer*/
	FTM0_init();
	FTM0_CH1_PWM_init();
	FTM1_init();
//	reset_counter();
	/*init ADC*/
	ADC_init();
	/*init CAN*/ 
	FLEXCAN0_init();
	
	
	while(1){
		
		FLEXCAN0_receive_msg (); /* Read message */
		convertAdcChan(12); /* Convert Channel AD12 to pot on EVB */
		while(adc_complete()==0){} /* Wait for conversion complete flag */
		set_value(REC.DATA[0]);
		if(enable_transmit) 
		{
#if (NODE_A)
			A_TO_B();
#else 
			B_TO_A();
#endif
		}
/*NODE A FOR BOARD 1*/
#if (NODE_A)
		/*Switch 2*/
		if( ((PTC->PDIR & (1<<12)) == (1<<12)))
		{
			stateBtn1 = 1;//ON
		}
		/*Switch 3*/
		if( ((PTC->PDIR & (1<<13)) == (1<<13))) 
		{
			stateBtn1 = 0;//OFF
		}	
		
		if(stateBtn1 ==1 && enable_transmit==0)//if ON
		{
			PTD->PDDR &= ~(1<<16); /* Port D16: Data direction = output */
			start_FTM0_counter();
			start_FTM1_counter();
			FLEXCAN0_transmit_msg(read_adc_chx(),0x00000001); /* Transmit message */
			enable_transmit=1;
		}
		if(stateBtn1==0)
		{
			stop_FTM0_counter();
			stop_FTM1_counter();
			PTD->PDDR |= 1<<16; /* Port D16: Data direction = output */
			FLEXCAN0_transmit_msg(read_adc_chx(),0x00000000);
			enable_transmit=0;
		}
		else
		{}
		
/*NODE B FOR BOARD 2*/
#else
		if (REC.DATA[1]== 0x1u)
		{
				start_FTM0_counter();
				start_FTM1_counter();	
				PTD->PDDR &= ~(1<<16); /* Port D16: Data direction = output */
				enable_transmit=1;
		}
		else 
		{
				stop_FTM0_counter();
				stop_FTM1_counter();
				PTD->PDDR |= 1<<16; /* Port D16: Data direction = output */
				enable_transmit=0;
		}
#endif
	}
}


