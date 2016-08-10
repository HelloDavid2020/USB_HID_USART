
#include"usart1.h"
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


