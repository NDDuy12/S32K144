#ifndef TIMER_H_
#define TIMER_H_
#include "stdint.h"
#define NODE_A 1 //1: A, 0: B


void FTM0_init(void);
void FTM0_CH1_PWM_init(void);
void start_FTM0_counter (void);
void set_value(uint8_t value);
int get_counter(void);
#endif 