#ifndef _USART1_H
#define _USART1_H

#include"stm32f10x.h"
#include"stdio.h"

int fputc(int ch,FILE *f);
//--��������ӡ�stdio.h�����c�ı�׼�������ͷ�ļ����������fputc���ǰ�������
//--��λ�Ҫ���target/use microLib��ʹ�ñ������ṩ��c΢�⣬����ʹstdio������ʹ��printf�������ض���fputc
void USART1_Config(void);
void USART1_printf(USART_TypeDef* USARTx, uint8_t *Data,...);
uint8_t USART_Scanf(uint32_t value);
#endif
