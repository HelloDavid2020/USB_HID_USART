#include "user_delay.h"
__IO uint32_t TimingDelay=0; 
void Init_SysTick(void) 
{ 
	if(SysTick_Config(SystemCoreClock / 1000)) //ע�⣺3.5���� SystemFrequency �� SystemCoreClock ȡ���� 
		while(1); 
} //��ʱ������ __IO uint32_t TimingDelay; 
void delay_ms(__IO uint32_t nTime) 
{ 
	TimingDelay = nTime; 
	while(TimingDelay != 0); 
} 




