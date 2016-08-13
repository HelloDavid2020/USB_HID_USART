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


//#include"stm32f10x_conf.h" //as this .h haven been included in stm32f10x.h

extern u8 Receive_Buffer[ReceiveLength];
extern u8 Transi_Buffer[SendLength];
extern u8 USB_ReceiveFlg;
uint32_t tx_cnt=0;
u8 tx_buf_data[50];
u8 tx_buf_len=0;

extern void USB_SendString(u8 *str);

void Delay(vu32 nCount);
u16 i=0;


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
	USB_PORT_SET(DISABLE);   //����ϵ�USB
	Delay(10000);
	USB_PORT_SET(ENABLE);	//U����ϵ�USB
	USART1_Init(115200);//
	USART3_Init(115200);// RS485
	printf("STM32 USB HID �շ�ʵ��\r\n");			
	Serial_PutString("\r\nHello RS485 \r\n");

	 while(1) //
	 {
		if (USB_ReceiveFlg == TRUE) //�յ�����λ�������ݺ󣬽�1S����һ�����ݸ�PC
		{				
			GPIO_Toggle(GPIOB, GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5);

			printf("\r\nUSB �յ�����:");
			Serial_PutString("\r\nRS485 TX\r\n");
			for(i=0;i<ReceiveLength;i++)
			{
				printf("%c",Receive_Buffer[i]);
			}
			sprintf((void*)tx_buf_data,"USB SEND DATA: %d",tx_cnt++);
			USB_SendString(tx_buf_data);
			USB_ReceiveFlg = 0x00;
		}
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


