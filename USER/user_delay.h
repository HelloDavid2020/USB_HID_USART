#ifndef _USER_DELAY_H_
#define _USER_DELAY_H_

#include "stm32f10x.h"
#include <stdbool.h>

extern __IO uint32_t TimingDelay; 
extern void Init_SysTick(void) ;
extern void delay_ms(__IO uint32_t nTime);

extern void SysTick_Handler(void) ;

#endif
