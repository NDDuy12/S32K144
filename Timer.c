#include "S32K144.h"
#include "GPIO.h"
#include "Timer.h"

//-------------------------------------------------------------------------------------------------------------
// Init Timer FTM0 : Mode PWM
//-------------------------------------------------------------------------------------------------------------
void FTM0_init(void) {
	PCC->PCCn[PCC_FTM0_INDEX] &= ~PCC_PCCn_CGC_MASK; /* Ensure clk disabled for config */
	PCC->PCCn[PCC_FTM0_INDEX] |= PCC_PCCn_PCS(0b001)/* Clock Src=1, 8 MHz SOSCDIV1_CLK */
	| PCC_PCCn_CGC_MASK; /* Enable clock for FTM regs */
	FTM0->MODE |= 0x4u; /* Write protect to registers disabled (default) */
#if (NODE_A)
	FTM0->SC = 0x00040007; 
	/* Enable PWM channel 2 output | LED RED*/
#else
FTM0->SC = 0x00010007; 
/* Enable PWM channel 0 output | LED BLUE */
/* TOIE (Timer Overflow Interrupt Ena) = 0 (default) */
/* CPWMS (Center aligned PWM Select) = 0 (default, up count) */
/* CLKS (Clock source) = 0 (default, no clock; FTM disabled) */
/* PS (Prescaler factor) = 7. Prescaler = 128 */
#endif
	FTM0->COMBINE = 0x00000000;/* FTM mode settings used: DECAPENx, MCOMBINEx, COMBINEx=0 */
	FTM0->POL = 0x00000000; /* Polarity for all channels is active high (default) */
	FTM0->MOD = 125  -1 ; /* FTM1 counter final value (used for PWM mode) */
	/* FTM1 Period = MOD-CNTIN+0x0001 ~= 62500 ctr clks */
	/* 8MHz /128 = 62.5kHz -> ticks -> 1Hz */
	FTM0->SC |= FTM_SC_TOIE_MASK;
}
//-------------------------------------------------------------------------------------------------------------
// Init Timer FTM0 to using timing
//-------------------------------------------------------------------------------------------------------------
void FTM1_init(void) {
	PCC->PCCn[PCC_FTM1_INDEX] &= ~PCC_PCCn_CGC_MASK; /* Ensure clk disabled for config */
	PCC->PCCn[PCC_FTM1_INDEX] |= PCC_PCCn_PCS(0b001)/* Clock Src=1, 8 MHz SOSCDIV1_CLK */
	| PCC_PCCn_CGC_MASK; /* Enable clock for FTM regs */
	FTM1->MODE |= 0x4u; /* Write protect to registers disabled (default) */
	FTM1->SC = 0x00040007;
	FTM1->COMBINE = 0x00000100;/* FTM mode settings used: DECAPENx, MCOMBINEx, COMBINEx=0 */
	FTM1->POL = 0x00000000; /* Polarity for all channels is active high (default) */
	FTM1->MOD = 62500 -1 ; /* FTM1 counter final value (used for PWM mode) */
	/* FTM1 Period = MOD-CNTIN+0x0001 ~= 62500 ctr clks */
	/* 8MHz /128 = 62.5kHz -> ticks -> 1Hz */
	FTM1->SC |= FTM_SC_TOIE_MASK;
}
//-------------------------------------------------------------------------------------------------------------
// Init Mode PWM FTM0
//-------------------------------------------------------------------------------------------------------------
void FTM0_CH1_PWM_init(void) {
#if (NODE_A) 
FTM0->CONTROLS[2].CnSC = 0x00000028; /* FTM0 ch2: edge-aligned PWM, low true pulses */
#else
FTM0->CONTROLS[0].CnSC = 0x00000028; /* FTM0 ch0: edge-aligned PWM, low true pulses */
 /* CHIE (Chan Interrupt Ena) = 0 (default) */
 /* MSB:MSA (chan Mode Select)=0b10, Edge Align PWM*/
 /* ELSB:ELSA (chan Edge/Level Select)=0b10, low true */
 /* FTM0 ch1 compare value (~75% duty cycle) */
#endif
}
//-------------------------------------------------------------------------------------------------------------
// Start/Stop timer
//-------------------------------------------------------------------------------------------------------------
void start_FTM0_counter(void) {
	FTM0->SC |= FTM_SC_CLKS(3);
	 /* Start FTM0 counter with clk source = external clock (SOSCDIV1_CLK)*/
}

void stop_FTM0_counter(void) {
	FTM0->SC &= ~(0x18u);
	/* Start FTM0 counter with clk source = external clock (SOSCDIV1_CLK)*/
}

void start_FTM1_counter (void) {

	FTM1->SC |= FTM_SC_CLKS(3);
 /* Start FTM0 counter with clk source = external clock (SOSCDIV1_CLK)*/
}

void stop_FTM1_counter(void) {
	FTM1->SC &= ~(0x18u);
	/* Start FTM0 counter with clk source = external clock (SOSCDIV1_CLK)*/
}
//-------------------------------------------------------------------------------------------------------------
// SET PWM CYCLE
//-------------------------------------------------------------------------------------------------------------
void set_value(uint8_t value)
{
	FTM0->CONTROLS[0].CnV = value;
	FTM0->CONTROLS[2].CnV = value;
}
//-------------------------------------------------------------------------------------------------------------
// GET TICKS TIMER VALUE
//-------------------------------------------------------------------------------------------------------------
int get_counter(void) {
	return FTM1->CNT;
}
//-------------------------------------------------------------------------------------------------------------
// RESET TICK
//-------------------------------------------------------------------------------------------------------------
void reset_counter(void)
{
//	FTM0->CNT=0;
	FTM1->CNT=0;
}
