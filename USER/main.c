//=================================================================================
//	Project_Name:	 stm32 DEMO
//	Description:		this is a template for creating a project
//	Date:		 2013年8月16日18:28:19（新加断言调试输出，串口输出，__FILE__ __LINE__）;
//	Made By:
//==================================================================================
//		MY STEPS
//step0: 修改工程名及说明信息；配置库文件（conf.h）；新建或者复制以前的文件到当前目录下，
//		 并添加用户(.c)文件到user；在main上添加相应的头文件；删除无用的库文件；
//srep1: 编写新建的用户文件，如果用到中断函数，还需修改it.c文件
//step2: 编写主函数（首先你要把你先前模块函数功能一步一步的实现，每个模块都调通再做扩展）；
//step3:默认我们是关闭 全功能断言调试的，调试时可以打开输出调试信息，可以有效的屏蔽函数参数
//		不合法的低级错误	
//		打开：去掉下面的注释        关闭：加上注释
//#define USE_FULL_ASSERT
//说明 
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




//软件断开或开启USB的电源，从而实现USB的开启或关闭
void USB_PORT_SET(FunctionalState NewState)
{  	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
	if(NewState)_SetCNTR(_GetCNTR()&(~(1<<1)));//退出断电模式
	else
	{	  
		_SetCNTR(_GetCNTR()|(1<<1));  // 断电模式
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

	USB_Cable_Config(ENABLE);  //硬件上拉D+，全速模式	
	USB_PORT_SET(DISABLE);   //软件断电USB
	Delay(10000);
	USB_PORT_SET(ENABLE);	//U软件上电USB
	USART1_Init(115200);//
	USART3_Init(115200);// RS485
	printf("STM32 USB HID 收发实验\r\n");			
	Serial_PutString("\r\nHello RS485 \r\n");

	 while(1) //
	 {
		if (USB_ReceiveFlg == TRUE) //收到后上位机的数据后，将1S发送一次数据给PC
		{				
			GPIO_Toggle(GPIOB, GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5);

			printf("\r\nUSB 收到数据:");
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

//-------------------用户自己定义的断言返回函数-----------------------------------
/****************************************************************** 
* Function name : assert_failed 
* Description : Reports the name of the source file and the source line number 
* where the assert_param error has occurred. 
* Input : - file: pointer to the source file name 
* - line: assert_param error line source number 
* Output : None 
* Return : None 
******************************************************************/
//默认关闭全功能断言调试
#ifdef USE_FULL_ASSERT 
void assert_failed(uint8_t* file, uint32_t line) 
{ 
/* User can add his own implementation to report the file name and line number, 
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */ 
/* Infinite loop */ 
	printf("出错位置：\r\n");
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


