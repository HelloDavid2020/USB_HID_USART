#ifndef _USART1_H
#define _USART1_H

#include"stm32f10x.h"
#include"stdio.h"

#define RS485_RX_EN() 	GPIO_ResetBits(GPIOB, GPIO_Pin_12)
#define RS485_RX_DIS() 	GPIO_SetBits(GPIOB, GPIO_Pin_12)



int fputc(int ch,FILE *f);
//--别忘了添加“stdio.h”这个c的标准输入输出头文件，下面这个fputc就是包含其中
//--其次还要添加target/use microLib：使用编译器提供的c微库，才能使stdio，才能使用printf，才能重定向fputc
void USART1_Config(void);
void USART1_printf(USART_TypeDef* USARTx, uint8_t *Data,...);
uint8_t USART_Scanf(uint32_t value);

extern void USART1_Init(uint32_t Baudrate);
extern void USART2_Init(uint32_t Baudrate);
extern void USART3_Init(uint32_t Baudrate);
extern void UART_sendPacket(USART_TypeDef* USARTx, uint8_t *buffer, uint8_t len);
extern void SerialPutChar(uint8_t c);
extern void Serial_PutString(uint8_t *s);

#endif
