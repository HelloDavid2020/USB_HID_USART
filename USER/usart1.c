
#include "usart1.h"
#include <stdarg.h>	 //	 *itoa(),USART1_printf()�����������õ������ͷ�ļ��Ķ��� ��Ҫ���ڽ��c
//�����У�������������ȷ�������


//============================================================================================
//--����  :	int fputc(int ch,FILE *f)	 
//--˵��  : �������Ǳ�������c�ṩ��printf�����������õģ���51��Ƭ����puts��printf��������
//			��������stm32����Ҫ��Щ�޸ģ������ض��򣬵��û��Լ���дc�⺯��ʱ�����������õ���⺯��ͬ����
//			�û�����ʱ����Ĭ�ϵ����û���д�ĺ������������ǾͿ����ض���⺯����
//			��printf�����ǵ���fputc����������������ģ��������Ǹ���stm32���ص��޸���fputc���
//			����printf�ʺ�stm32�Ĵ����ˡ�������ض���
//--����  :   ch f
//--����ֵ:   ch 
//--��Χ  : ��c�⺯��printf����
//--ʱ��  ��2012��9��22��21:22:42
//--��������ӡ�stdio.h�����c�ı�׼�������ͷ�ļ����������fputc���ǰ�������
//--��λ�Ҫ���target/use microLib��ʹ�ñ������ṩ��c΢�⣬����ʹstdio������ʹ��printf�������ض���fputc
//============================================================================================
int fputc(int ch,FILE *f)
{
	USART_SendData(USART1,(unsigned char) ch);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
	return (ch);
}




void UART_sendPacket(USART_TypeDef* USARTx, uint8_t *buffer, uint8_t len)
{

	uint8_t	sendIndex	=len;
	uint8_t	*pData	=NULL;
	pData = buffer;
	
	while(sendIndex--)
	{	
		USARTx->SR; //
		USART_SendData(USARTx,*pData++);
		while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
	}
}

// �~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~
/*******************************************************************************
  * @��������	SerialPutChar
  * @����˵��   ���ڷ���һ���ַ�
  * @�������   C:�跢�͵��ַ�
  * @�������   i��
  * @���ز���   ��
*******************************************************************************/
void SerialPutChar(uint8_t c)
{
		USART2->SR; // ��ֹ��һ�η���ʱ������

    USART_SendData(USART3, c);
    while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET)
    {
    }
}

/*******************************************************************************
  * @��������	SerialPutChar
  * @����˵��   ���ڷ���һ���ַ���
  * @�������   *s:�跢�͵��ַ���
  * @�������   ��
  * @���ز���   ��
*******************************************************************************/
void Serial_PutString(uint8_t *s)
{
		RS485_RX_DIS();

    while (*s != '\0')
    {
        SerialPutChar(*s);
        s++;
    }
		RS485_RX_EN();

}

//============================================================================================
//--����  :	void USART1_Config(void)	 
//--˵��  : ��Ҫ���ڳ�ʼ��USART1,����:115200-8-N-1	, ���Ծ�������޸�
//--����  :   �� 
//--����ֵ:   ��  
//--��Χ  : ����Χ���ã���ʼ��usart1 
//--ʱ��  ��2012��9��21��20:14:48
//============================================================================================
void USART1_Config()
{
	//gpio��usart��ʼ���ṹ�嶨��
	//��gpio����ʱ����������ʱ������Ҫ��ʼ��ͬʱgpio��ppp���裬��������ʹ�ܺͿ���ʱ��
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	//����gpio������ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

	//gpio��ʼ���ṹ���Ա��ֵ
	//��ʼ��PA��9�������������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//ִ�������gpio��ʼ��
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// PA.10��������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//USART��ʼ��
	//���������ã����ÿ⺯�������ǿ���ֱ���������ò����ʣ�������Ҫ���м���USARTDIV�ķ�Ƶ����
	USART_InitStructure.USART_BaudRate = 115200;
	
	//���ô��ڴ�����ֳ��������̰�������Ϊ��õ�8λ�ֳ���Ҳ��������Ϊ9λ��
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	
	//����ֹͣλ����ͨѶЭ���е�ֹͣλ����Ϊ1λ��
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	
	//������żУ��λ�������̲�������żУ��λ
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	
	//����Ӳ�������ơ�������Ӳ������
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	
	//���ô��ڵ�ģʽ��Ϊ������˫��ȫ˫��ͨѶ����Ҫ��Rx��Txģʽ��������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	//ִ������Ĳ���
	USART_Init(USART1, &USART_InitStructure);
	
	//����USART_Cmd() ʹ��USART1���衣��ʹ������ʱ������Ҫʹ����ʱ�ӣ���Ҫ���ô˺���ʹ������ſ�������ʹ�á�
	USART_Cmd(USART1, ENABLE);
}

void USART1_Init(uint32_t Baudrate)
{
	GPIO_InitTypeDef         GPIO_InitStructure;
// 	NVIC_InitTypeDef         NVIC_InitStructure;
	USART_InitTypeDef        USART_InitStructure;
	
	
	/* Enable USART1, GPIOA and AFIO clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	/* Enable USART1 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

		
	/* Configure USART1 Tx (PA.09) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);	

  /* Configure USART1 Rx (PA.10) as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_DeInit(USART1);
	
  USART_InitStructure.USART_BaudRate = Baudrate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART1, &USART_InitStructure);


	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
// USART_ITConfig(USART1, USART_IT_TXE | USART_IT_RXNE, ENABLE);




//	//����UART1�������ݶ���
//	QueueCreate((void *)UART1SendBuf, sizeof(UART1SendBuf), NULL, (uint8_t (*)())UartWriteFull);
// 	//����UART1�������ݶ���
//	QueueCreate((void *)UART1RecvBuf, sizeof(UART1RecvBuf), NULL, (uint8_t (*)())UartWriteFull);


/* Enable USART1 */
  USART_Cmd(USART1, ENABLE);
	USART_ClearFlag(USART1, USART_FLAG_TC);
}



void USART2_Init(uint32_t Baudrate)
{

	GPIO_InitTypeDef         GPIO_InitStructure;
// 	NVIC_InitTypeDef         NVIC_InitStructure;
	USART_InitTypeDef        USART_InitStructure;
//	DMA_InitTypeDef DMA_InitStructure;
	
  /* Enable USART2, GPIOA and AFIO clocks */
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA| RCC_APB2Periph_AFIO, ENABLE);
  /* Enable USART2 clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

  /* Enable the USART2 Pins Software Remapping */
//   GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);

	/* Configure USART2 Tx (PA.02) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);	

  /* Configure USART2 Rx (PA.03) as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);



//	/* Configure USART2 Tx (PD5) as alternate function push-pull */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//  GPIO_Init(GPIOD, &GPIO_InitStructure);	

//  /* Configure USART2 Rx (PD6) as input floating */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//  GPIO_Init(GPIOD, &GPIO_InitStructure);




///* Enable DMA1 clock */
//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
//	DMA_DeInit(DMA1_Channel6); 
//	/* Periph addr is usart rx data register address  */
//	DMA_InitStructure.DMA_PeripheralBaseAddr    = USART2_BASE+4;
//	DMA_InitStructure.DMA_MemoryBaseAddr        = (uint32_t)gps_buffer;
//	DMA_InitStructure.DMA_BufferSize            = NMEA_COUNT_MAX;

//	/* Usart rx data reg need not increment, rx buffer nned */
//	DMA_InitStructure.DMA_PeripheralInc         = DMA_PeripheralInc_Disable;
//	DMA_InitStructure.DMA_MemoryInc             = DMA_MemoryInc_Enable;
//	/* Once transfer one byte and using cycle mode  */
//	DMA_InitStructure.DMA_PeripheralDataSize    = DMA_PeripheralDataSize_Byte;
//	DMA_InitStructure.DMA_MemoryDataSize        = DMA_MemoryDataSize_Byte;
//	DMA_InitStructure.DMA_Mode                  = DMA_Mode_Circular;           
//	/* Using highest priority, transfer direction is perihp to memory(USART to memory)  */    
//	DMA_InitStructure.DMA_Priority              = DMA_Priority_VeryHigh;
//	DMA_InitStructure.DMA_DIR                   = DMA_DIR_PeripheralSRC;
//	DMA_InitStructure.DMA_M2M                   = DMA_M2M_Disable;
//	DMA_Init(DMA1_Channel6, &DMA_InitStructure);	
//	
//	/* Enable USART2 RX DMA channel */
//	DMA_Cmd(DMA1_Channel6, ENABLE);	
	
	USART_DeInit(USART2);
	
  USART_InitStructure.USART_BaudRate = Baudrate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USART2, &USART_InitStructure);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
//  USART_ITConfig(USART2, USART_IT_TXE, ENABLE);

	/* Enable USART DMA RX request */
//	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);
//	
//	QueueCreate((void *)UART2SendBuf, sizeof(UART2SendBuf), NULL, (uint8_t (*)())UartWriteFull);
//	QueueCreate((void *)UART2RecvBuf, sizeof(UART2RecvBuf), NULL, (uint8_t (*)())UartWriteFull);
	
  /* Enable USART2 */
  USART_Cmd(USART2, ENABLE);
//	USART_ClearFlag(USART2, USART_FLAG_TC);
}


// RS485
void USART3_Init(uint32_t Baudrate)
{

	GPIO_InitTypeDef         GPIO_InitStructure;
// 	NVIC_InitTypeDef         NVIC_InitStructure;
	USART_InitTypeDef        USART_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO , ENABLE);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO , ENABLE);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	// USART3 remap
//	GPIO_PinRemapConfig(GPIO_FullRemap_USART3, ENABLE);
	
	
//	
//	/* Configure USART3 Tx (PD.08) as alternate function push-pull */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//  GPIO_Init(GPIOD, &GPIO_InitStructure);	

//  /* Configure USART3 Rx (PD.09) as input floating */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//  GPIO_Init(GPIOD, &GPIO_InitStructure);

	
	/* Configure USART3 Tx (PD.08) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);	

  /* Configure USART3 Rx (PD.09) as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);	
	RS485_RX_EN();

	USART_DeInit(USART3);
	
  USART_InitStructure.USART_BaudRate = Baudrate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USART3, &USART_InitStructure);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
// USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
//	
//	QueueCreate((void *)UART3SendBuf, sizeof(UART3SendBuf), NULL, (uint8_t (*)())UartWriteFull);
//	QueueCreate((void *)UART3RecvBuf, sizeof(UART3RecvBuf), NULL, (uint8_t (*)())UartWriteFull);

  /* Enable USART3 */
  USART_Cmd(USART3, ENABLE);
	USART_ClearFlag(USART3, USART_FLAG_TC);
}

void UART4_Init(uint32_t Baudrate)
{

  GPIO_InitTypeDef GPIO_InitStructure; 
  USART_InitTypeDef USART_InitStructure; 	
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);//UART4

	
	/* Configure UART4 Tx (PC.10) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);	

  /* Configure UART4 Rx (PC.11) as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

	
	USART_DeInit(UART4);
	
  USART_InitStructure.USART_BaudRate = Baudrate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(UART4, &USART_InitStructure);
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
// 	USART_ITConfig(UART4, USART_IT_TXE, ENABLE);
  /* Enable UART4 */
  USART_Cmd(UART4, ENABLE);
	USART_ClearFlag(UART4, USART_FLAG_TC);
}


void UART5_Init(uint32_t Baudrate)
{

  GPIO_InitTypeDef GPIO_InitStructure; 
  USART_InitTypeDef USART_InitStructure; 	
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);//UART4

	
	/* Configure UART5 Tx (PC.12) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);	
	
  /* Configure UART5 Rx (PD.02) as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

	
	USART_DeInit(UART5);
	
  USART_InitStructure.USART_BaudRate = Baudrate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(UART5, &USART_InitStructure);
// USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
// USART_ITConfig(UART5, USART_IT_TXE, ENABLE);
  /* Enable UART5 */
  USART_Cmd(UART5, ENABLE);
	USART_ClearFlag(UART5, USART_FLAG_TC);
}



//============================================================================================
//--����  :	static char *itoa(int value, char *string, int radix)	
//--˵��  : ��Ҫ���ڳ�ʼ��USART1,����:115200-8-N-1	, ���Ծ�������޸�
//--����  :   value : Ҫת�������������磺1��2��3
//			  string: ת������ַ���
//			  radix : radix =10 ��ʾ10���ƣ��������Ϊ0
//--����ֵ:   	return string;  
//--��Χ  : ������USART1_printf()���ã�������ת�����ַ��� ,���ҷ����ַ���
//============================================================================================
static char *itoa(int value, char *string, int radix)	
//˵���˺���Ϊ�ڲ�������static��������ֵΪָ���ͣ�*�����ַ�����
{
	int     i, d;
	int     flag = 0;
	char    *ptr = string;
	
	/* This implementation only works for decimal numbers. */
	if (radix != 10)
	{
	    *ptr = 0;
	    return string;
	}
	
	if (!value)
	{
	    *ptr++ = 0x30;
	    *ptr = 0;
	    return string;
	}
	
	/* if this is a negative value insert the minus sign. */
	if (value < 0)
	{
	    *ptr++ = '-';
	
	    /* Make the value positive. */
	    value *= -1;
	}
	
	for (i = 10000; i > 0; i /= 10)
	{
	    d = value / i;
	
	    if (d || flag)
	    {
	        *ptr++ = (char)(d + 0x30);
	        value -= (d * i);
	        flag = 1;
	    }
	}
	
	/* Null terminate the string. */
	*ptr = 0;
	
	return string;

} /* NCL_Itoa */
//============================================================================================
//--����  :	void USART1_printf(USART_TypeDef* USARTx, uint8_t *Data,...)	
//--˵��  : ���ڴ�ӡ������谴��ָ���ĸ�ʽ
//--����  :  USARTx ����ͨ��������ֻ�õ��˴���1����USART1
//			  Data   Ҫ���͵����ڵ����ݵ�ָ��
//			  -...    ��������
//--����ֵ:   	�� 
//--��Χ  : ���ڴ��������ָ����ʽ������
//			����ַ�������1����USART1_printf( USART1, "\r\n this is a demo \r\n" );
//            		 	��2����USART1_printf( USART1, "\r\n %s \r\n","jiaohaitao" );  //��ʵ�Ӳ���%s�����ʽ������ν
//							 ���Ը�������Ĵ���֪������%s�Ļ������ַ���ʽ��������ӵĻ�Ĭ���ַ������
//						��3����USART1_printf( USART1, "\r\n %d %d\r\n", i,10 );�ڡ�������Ӹ�ʽ������Ӧ�����������������ֵ��
//						 (4) : USART1_printf( USART1, "\r\n %d\r\n",10 );
//            		 
void USART1_printf(USART_TypeDef* USARTx, uint8_t *Data,...)  //Ϊʲô���������������������ⲻ֪��
{
	const char *s;
	int d;   
	char buf[16];
	
	va_list ap;
	va_start(ap, Data);	   //�Ҹо���������������ǰ��������е���һ������ϵ�д���������������
	 //��������ã���USART1��printf���������������е�һ�������еĲ�������Data�������ڵ���ʱ�����Ǻ���Ĳ�����һ�����ơ�
	while ( *Data != 0)     // �ж��Ƿ񵽴��ַ���������
	{				                          
		if ( *Data == 0x5c )  //'\'
	{									  
	switch ( *++Data )
	{
		case 'r':							          //�س���
			USART_SendData(USARTx, 0x0d);
			Data ++;
		break;
		
		case 'n':							          //���з�
			USART_SendData(USARTx, 0x0a);	
			Data ++;
		break;
		
		default:
			Data ++;
		break;
	}			 
	}
	else if ( *Data == '%')
	{									  //
	switch ( *++Data )
	{				
		case 's':										  //�ַ���
			s = va_arg(ap, const char *);
	for ( ; *s; s++) 
	{
		USART_SendData(USARTx,*s);
		while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
	}
		Data++;
		break;
	case 'd':										//ʮ����
	d = va_arg(ap, int);
	itoa(d, buf, 10);
	for (s = buf; *s; s++) 
	{
		USART_SendData(USARTx,*s);
		while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
	}
	Data++;
	break;
		 default:
				Data++;
		    break;
	}		 
	} /* end of else if */
	else USART_SendData(USARTx, *Data);		//++ ��* ���ȼ���++���ȼ����ߡ����Բ��ܣ�*Data++��=��*��Data++����
	//�����Ļ��� �ͻ��õ�һ���ַ�������
	Data++;
	while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
	}
}
//========================================================================================================================
/**
  * @brief  Gets numeric values from the hyperterminal.
  * @param  None
  * @retval None
  */
uint8_t USART_Scanf(uint32_t value)
{
  uint32_t index = 0;
  uint32_t tmp[2] = {0, 0};

  while (index < 2)
  {
    /* Loop until RXNE = 1 */
    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
    {}
    tmp[index++] = (USART_ReceiveData(USART1));
    if ((tmp[index - 1] < 0x30) || (tmp[index - 1] > 0x39))
    {
      printf("\n\rPlease enter valid number between 0 and 9");
      index--;
    }
  }
  /* Calculate the Corresponding value */
  index = (tmp[1] - 0x30) + ((tmp[0] - 0x30) * 10);
  /* Checks */
  if (index > value)
  {
    printf("\n\rPlease enter valid number between 0 and %d", value);
    return 0xFF;
  }
  return index;
}


