
#include"usart1.h"
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


