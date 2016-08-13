
#include "usart1.h"
#include <stdarg.h>	 //	 *itoa(),USART1_printf()这两个函数用到了这个头文件的定义 主要用于解决c
//语言中，函数，参数不确定的情况


//============================================================================================
//--函数  :	int fputc(int ch,FILE *f)	 
//--说明  : 本函数是被编译器c提供的printf（）函数调用的，和51单片机里puts和printf（）相似
//			不过，在stm32中需要做些修改，即：重定向，当用户自己重写c库函数时，当编译器用到与库函数同名的
//			用户函数时，会默认调用用户编写的函数，这样我们就可以重定向库函数了
//			而printf函数是调用fputc这个函数来完成输出的，所以我们根据stm32的特点修改下fputc这个
//			就是printf适合stm32的串口了。即完成重定向。
//--参数  :   ch f
//--返回值:   ch 
//--范围  : 被c库函数printf调用
//--时间  ：2012年9月22日21:22:42
//--别忘了添加“stdio.h”这个c的标准输入输出头文件，下面这个fputc就是包含其中
//--其次还要添加target/use microLib：使用编译器提供的c微库，才能使stdio，才能使用printf，才能重定向fputc
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

// ▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇
/*******************************************************************************
  * @函数名称	SerialPutChar
  * @函数说明   串口发送一个字符
  * @输入参数   C:需发送的字符
  * @输出参数   i无
  * @返回参数   无
*******************************************************************************/
void SerialPutChar(uint8_t c)
{
		USART2->SR; // 防止第一次发送时的乱码

    USART_SendData(USART3, c);
    while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET)
    {
    }
}

/*******************************************************************************
  * @函数名称	SerialPutChar
  * @函数说明   串口发送一个字符串
  * @输入参数   *s:需发送的字符串
  * @输出参数   无
  * @返回参数   无
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
//--函数  :	void USART1_Config(void)	 
//--说明  : 主要用于初始化USART1,例如:115200-8-N-1	, 可以具体情况修改
//--参数  :   无 
//--返回值:   无  
//--范围  : 被外围调用，初始化usart1 
//--时间  ：2012年9月21日20:14:48
//============================================================================================
void USART1_Config()
{
	//gpio，usart初始化结构体定义
	//当gpio复用时（开启外设时），需要初始化同时gpio和ppp外设，并对他们使能和开启时钟
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	//开启gpio和外设时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

	//gpio初始化结构体成员赋值
	//初始化PA。9，复用推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//执行上面的gpio初始化
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// PA.10浮空输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//USART初始化
	//波特率设置，利用库函数，我们可以直接这样配置波特率，而不需要自行计算USARTDIV的分频因子
	USART_InitStructure.USART_BaudRate = 115200;
	
	//配置串口传输的字长。本例程把它设置为最常用的8位字长，也可以设置为9位。
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	
	//配置停止位。把通讯协议中的停止位设置为1位。
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	
	//配置奇偶校验位。本例程不设置奇偶校验位
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	
	//配置硬件流控制。不采用硬件流。
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	
	//配置串口的模式。为了配置双线全双工通讯，需要把Rx和Tx模式都开启。
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	//执行上面的操作
	USART_Init(USART1, &USART_InitStructure);
	
	//调用USART_Cmd() 使能USART1外设。在使用外设时，不仅要使能其时钟，还要调用此函数使能外设才可以正常使用。
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




//	//创建UART1发送数据队列
//	QueueCreate((void *)UART1SendBuf, sizeof(UART1SendBuf), NULL, (uint8_t (*)())UartWriteFull);
// 	//创建UART1接收数据队列
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
//--函数  :	static char *itoa(int value, char *string, int radix)	
//--说明  : 主要用于初始化USART1,例如:115200-8-N-1	, 可以具体情况修改
//--参数  :   value : 要转换的整形数；如：1、2、3
//			  string: 转换后的字符串
//			  radix : radix =10 表示10进制，其他结果为0
//--返回值:   	return string;  
//--范围  : 被函数USART1_printf()调用，将数字转化成字符串 ,并且返回字符串
//============================================================================================
static char *itoa(int value, char *string, int radix)	
//说明此函数为内部函数（static）；返回值为指针型（*）（字符串）
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
//--函数  :	void USART1_printf(USART_TypeDef* USARTx, uint8_t *Data,...)	
//--说明  : 串口打印输出，需按照指定的格式
//--参数  :  USARTx 串口通道，这里只用到了串口1，即USART1
//			  Data   要发送到串口的内容的指针
//			  -...    其他参数
//--返回值:   	无 
//--范围  : 用于串口输出，指定格式的数据
//			输出字符串：（1）：USART1_printf( USART1, "\r\n this is a demo \r\n" );
//            		 	（2）：USART1_printf( USART1, "\r\n %s \r\n","jiaohaitao" );  //其实加不加%s这个格式符无所谓
//							 可以根据下面的代码知道，加%s的话将按字符格式输出，不加的话默认字符输出。
//						（3）：USART1_printf( USART1, "\r\n %d %d\r\n", i,10 );在“”中添加格式符，对应“，”后面变量，数值，
//						 (4) : USART1_printf( USART1, "\r\n %d\r\n",10 );
//            		 
void USART1_printf(USART_TypeDef* USARTx, uint8_t *Data,...)  //为什么后面参数：。。。。这个这不知道
{
	const char *s;
	int d;   
	char buf[16];
	
	va_list ap;
	va_start(ap, Data);	   //我感觉这个函数的作用是把主函数中的下一个参数系列传个本函数参数的
	 //上面的作用：把USART1—printf（“”。。。）中第一个“”中的参数传给Data，下面在调用时，就是后面的参数，一次类推。
	while ( *Data != 0)     // 判断是否到达字符串结束符
	{				                          
		if ( *Data == 0x5c )  //'\'
	{									  
	switch ( *++Data )
	{
		case 'r':							          //回车符
			USART_SendData(USARTx, 0x0d);
			Data ++;
		break;
		
		case 'n':							          //换行符
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
		case 's':										  //字符串
			s = va_arg(ap, const char *);
	for ( ; *s; s++) 
	{
		USART_SendData(USARTx,*s);
		while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
	}
		Data++;
		break;
	case 'd':										//十进制
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
	else USART_SendData(USARTx, *Data);		//++ 和* 优先级，++优先级更高。所以不能（*Data++）=（*（Data++））
	//这样的话， 就会让第一个字符丢掉。
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


