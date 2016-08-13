#include "user_delay.h"
__IO uint32_t TimingDelay=0; 
void Init_SysTick(void) 
{ 
	if(SysTick_Config(SystemCoreClock / 1000)) //注意：3.5库中 SystemFrequency 被 SystemCoreClock 取代。 
		while(1); 
} //延时函数： __IO uint32_t TimingDelay; 
void delay_ms(__IO uint32_t nTime) 
{ 
	TimingDelay = nTime; 
	while(TimingDelay != 0); 
} 




