//=================================================================================
//	Project_Name:	 stm32 DEMO
//	Description:		this is a template for creating a project
//	Date:		 2013��8��16��18:28:19���¼Ӷ��Ե�����������������__FILE__ __LINE__��;
//	Made By:
//==================================================================================
//		MY STEPS
//step0: �޸Ĺ�������˵����Ϣ�����ÿ��ļ���conf.h�����½����߸�����ǰ���ļ�����ǰĿ¼�£�
//		 ������û�(.c)�ļ���user����main�������Ӧ��ͷ�ļ���ɾ�����õĿ��ļ���
//srep1: ��д�½����û��ļ�������õ��жϺ����������޸�it.c�ļ�
//step2: ��д��������������Ҫ������ǰģ�麯������һ��һ����ʵ�֣�ÿ��ģ�鶼��ͨ������չ����
//step3:Ĭ�������ǹر� ȫ���ܶ��Ե��Եģ�����ʱ���Դ����������Ϣ��������Ч�����κ�������
//		���Ϸ��ĵͼ�����	
//		�򿪣�ȥ�������ע��        �رգ�����ע��
//#define USE_FULL_ASSERT
//˵�� 
#include "stm32f10x.h"
#include "usart1.h"
#include "usb_lib.h"
#include "hw_config.h"
#include "user_gpio.h"
#include "user_rs485.h"
#include "user_delay.h"
//#include"stm32f10x_conf.h" //as this .h haven been included in stm32f10x.h

extern u8 Receive_Buffer[ReceiveLength];
extern u8 Transi_Buffer[SendLength];
extern u8 USB_ReceiveFlg;
uint32_t tx_cnt=0;
u8 tx_buf_data[50];
u8 tx_buf_len=0;

extern void USB_SendString(u8 *str);
void USB_sendPacket(uint8_t *buffer, uint8_t len);
void Delay(vu32 nCount);
u16 i=0;
void report_data(void);

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures NVIC and Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void user_nvic_config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x00);

  /* 2 bits for Preemption Priority and 2 bits for Sub Priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
	

	/* Enable the USB interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* Enable the USB Wake-up interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USBWakeUp_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;

	NVIC_Init(&NVIC_InitStructure);   
	
	
	
	
}




//����Ͽ�����USB�ĵ�Դ���Ӷ�ʵ��USB�Ŀ�����ر�
void USB_PORT_SET(FunctionalState NewState)
{  	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
	if(NewState)_SetCNTR(_GetCNTR()&(~(1<<1)));//�˳��ϵ�ģʽ
	else
	{	  
		_SetCNTR(_GetCNTR()|(1<<1));  // �ϵ�ģʽ
		GPIOA->CRH&=0XFFF00FFF;
		GPIOA->CRH|=0X00033000;
		GPIO_ResetBits(GPIOA,GPIO_Pin_12);    		  
	}
}  		
int main(void)
{
	user_nvic_config();
	
	led_gpio_init();
	
	USB_GPIO_Configuration();
	//USB_Interrupts_Config();
	Set_USBClock();    
	USB_Init();
	USB_ReceiveFlg = FALSE;

	USB_Cable_Config(ENABLE);  //Ӳ������D+��ȫ��ģʽ	
//	USB_PORT_SET(DISABLE);   //����ϵ�USB
//	Delay(10000);
//	USB_PORT_SET(ENABLE);	//U����ϵ�USB
	USART1_Init(115200);//
	USART3_Init(115200);// RS485
	Init_SysTick();
	
	//printf("STM32 USB HID �շ�ʵ��\r\n");			
	//Serial_PutString("\r\nHello RS485 \r\n");		
	delay_ms(200);
	
scan_device();


get_device_value(&biaotou[0],100);
get_device_value(&biaotou[1],100);
get_device_value(&biaotou[2],100);
//get_device_value(biaotou[3].address,100,biaotou[3]);

	 while(1) //
	 {
//		if (USB_ReceiveFlg == TRUE) //�յ�����λ�������ݺ󣬽�1S����һ�����ݸ�PC
//		{				
//			GPIO_Toggle(GPIOB, GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5);

////			printf("\r\nUSB �յ�����:");
////			
////			for(i=0;i<ReceiveLength;i++)
////			{
////				printf("%c",Receive_Buffer[i]);
////			}
//			sprintf((void*)tx_buf_data,"source_add: <%02d> <%02d> <%02d> <%02d>",source_add[0],source_add[1],source_add[2],source_add[3]);
//			USB_SendString(tx_buf_data);
//			USB_ReceiveFlg = 0x00;
//		}
		 
		 
		get_device_value(&biaotou[0],100);
		get_device_value(&biaotou[1],100);
		get_device_value(&biaotou[2],100); 
		get_device_value(&biaotou[3],100); 

report_data();
//		sprintf((void*)tx_buf_data,"<%02d %.02f> <%02d %.02f> <%02d %.02f> <%02d %.02f>",
//		biaotou[0].address,biaotou[0].real_value,biaotou[1].address,biaotou[1].real_value,biaotou[2].address,biaotou[2].real_value,biaotou[3].address,biaotou[3].real_value);

//		USB_SendString(tx_buf_data);

		GPIO_Toggle(GPIOB, GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5);
		delay_ms(200);

	 }
}
void USB_SendString(u8 *str)
{
     u8 ii=0;   
     while(*str)
     {
         Transi_Buffer[ii++]=*(str++);
         if (ii == SendLength) break;
     }
     UserToPMABufferCopy(Transi_Buffer, ENDP2_TXADDR, SendLength);
     SetEPTxValid(ENDP2);
}



void USB_sendPacket(uint8_t *buffer, uint8_t len)
{
	uint8_t ii=0;   

	uint8_t	sendIndex	=len;
	uint8_t	*pData	=NULL;
	pData = buffer;

	while(sendIndex--)
	{	
		Transi_Buffer[ii++]=*(pData++);
	}
  UserToPMABufferCopy(Transi_Buffer, ENDP2_TXADDR, len);
  SetEPTxValid(ENDP2);
}

void report_data(void)
{
	static uint8_t cnt=0;
	uint8_t *ptr=NULL;
	ptr=tx_buf_data;
	tx_buf_data[0]=0xF1;
	tx_buf_data[1]=0xF2;
	tx_buf_data[2]=0xF3;
	
	tx_buf_data[3]=biaotou[0].address;
	tx_buf_data[4]=biaotou[0].type;
	*(int16_t*)(tx_buf_data+5)=biaotou[0].ivalue;  // ��ѹ��1

	tx_buf_data[7]=biaotou[1].address;
	tx_buf_data[8]=biaotou[1].type;
	*(int16_t*)(tx_buf_data+9)=biaotou[1].ivalue;  // ��ѹ��1

	tx_buf_data[11]=biaotou[2].address;
	tx_buf_data[12]=biaotou[2].type;	
	*(int16_t*)(tx_buf_data+13)=biaotou[2].ivalue;  // ��ѹ��1


	tx_buf_data[15]=biaotou[3].address;
	tx_buf_data[16]=biaotou[3].type;
	*(int16_t*)(tx_buf_data+17)=biaotou[3].ivalue;  // ��ѹ��1

//	*ptr+2=0x01;  // ��ѹ��2
//	*ptr++=0x02;

//	*ptr++=0x01;  // ������1 
//	*ptr++=0x02;

//	*ptr++=0x01;  // ������2 
//	*ptr++=cnt++;


	tx_buf_data[19]=0xBE;
	tx_buf_data[20]=0xBF;//	

	tx_buf_len =21;	
	USB_sendPacket(tx_buf_data,tx_buf_len);

}

/*******************************************************************************
* Function Name  : Delay
* Description    : Inserts a delay time.
* Input          : nCount: specifies the delay time length.
* Output         : None
* Return         : None
*******************************************************************************/
void Delay(vu32 nCount)
{
  for(; nCount!= 0;nCount--);
}

//-------------------�û��Լ�����Ķ��Է��غ���-----------------------------------
/****************************************************************** 
* Function name : assert_failed 
* Description : Reports the name of the source file and the source line number 
* where the assert_param error has occurred. 
* Input : - file: pointer to the source file name 
* - line: assert_param error line source number 
* Output : None 
* Return : None 
******************************************************************/
//Ĭ�Ϲر�ȫ���ܶ��Ե���
#ifdef USE_FULL_ASSERT 
void assert_failed(uint8_t* file, uint32_t line) 
{ 
/* User can add his own implementation to report the file name and line number, 
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */ 
/* Infinite loop */ 
	printf("����λ�ã�\r\n");
	printf("FILE:%s\r\n",__FILE__);
	printf("LINE:%d\r\n",__LINE__);
}
#endif 


void NVIC_Configuration(void)
{    
  //NVIC_InitTypeDef NVIC_InitStructure;
#ifdef  VECT_TAB_RAM  
  /* Set the Vector Table base location at 0x20000000 */ 
     NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else  /* VECT_TAB_FLASH  */
  /* Set the Vector Table base location at 0x08000000 */ 
     NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
#endif 
}


