/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : hw_config.c
* Author             : MCD Application Team
* Version            : V2.2.1
* Date               : 09/22/2008
* Description        : Hardware Configuration & Setup
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

#include "hw_config.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#define BOOL bool
ErrorStatus HSEStartUpStatus;
void RTC_Configuration(void);
void Timer2Configuration(void);

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define USB_DISCONNECT                      GPIOE  			//usb的开关引脚，1.5k上拉电阻的控制引脚
#define USB_DISCONNECT_PIN                  GPIO_Pin_3		//对于没有接控制引脚的，可以定义为空
#define USB_DISCONNECT_RCC  				RCC_APB2Periph_GPIOE
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : Set_USBClock
* Description    : Configures USB Clock input (48MHz).
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Set_USBClock(void)
{
  // Select USBCLK source 
     RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);  // 72MHZ sysclk
  //  RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_Div1);

  /* Enable USB clock */
     RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);
}

/*******************************************************************************
* Function Name  : Enter_LowPowerMode.
* Description    : Power-off system clocks and power while entering suspend mode.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Enter_LowPowerMode(void)
{
  /* Set the device state to suspend */
	bDeviceState = SUSPENDED;
}

/*******************************************************************************
* Function Name  : Leave_LowPowerMode.
* Description    : Restores system clocks and power while exiting suspend mode.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Leave_LowPowerMode(void)
{
     DEVICE_INFO *pInfo = &Device_Info;
  
  /* Set the device state to the correct state */
     if (pInfo->Current_Configuration != 0)
     {
    /* Device configured */
	 bDeviceState = CONFIGURED;
     }
     else 
     {
	 bDeviceState = ATTACHED;
     }
}

/*******************************************************************************
* Function Name  : USB_Interrupts_Config.
* Description    : Configures the USB interrupts.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void USB_Interrupts_Config(void)
{
//      NVIC_InitTypeDef NVIC_InitStructure; 
//      NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
//      NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//      NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//      NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//      NVIC_Init(&NVIC_InitStructure);
// 	
// 	/* Enable the USB Wake-up interrupt */
// 	NVIC_InitStructure.NVIC_IRQChannel = USBWakeUp_IRQn;
// 	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
// 	NVIC_Init(&NVIC_InitStructure);  
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

	/* 2 bit for pre-emption priority, 2 bits for subpriority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	/* Configure the EXTI line 18 connected internally to the USB IP */
	EXTI_ClearITPendingBit(EXTI_Line18);
	EXTI_InitStructure.EXTI_Line = EXTI_Line18; // USB resume from suspend mode
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure); 	 

	/* Enable the USB interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* Enable the USB Wake-up interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USBWakeUp_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Init(&NVIC_InitStructure);   
}

/*******************************************************************************
* Function Name  : USB_Cable_Config.
* Description    : Software Connection/Disconnection of USB Cable.
* Input          : NewState: new state.
* Output         : None.
* Return         : None
*******************************************************************************/
void USB_Cable_Config (FunctionalState NewState)
{ 

 	if (NewState)
	{
		GPIO_ResetBits(USB_DISCONNECT, USB_DISCONNECT_PIN);
	}
	else
	{
		GPIO_SetBits(USB_DISCONNECT, USB_DISCONNECT_PIN);
	}
}

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the different GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_GPIO_Configuration(void)
{
	 GPIO_InitTypeDef GPIO_InitStructure;

     RCC_APB2PeriphClockCmd(USB_DISCONNECT_RCC, ENABLE);
     GPIO_InitStructure.GPIO_Pin = USB_DISCONNECT_PIN;
     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
     GPIO_Init(USB_DISCONNECT, &GPIO_InitStructure);
}
void DisableClock(void)
{
     RCC_APB1PeriphClockCmd(RCC_APB1Periph_ALL,DISABLE);
     RCC_APB2PeriphClockCmd(RCC_APB2Periph_ALL,DISABLE);
}

/*******************************************************************************
* Function Name  : Get_SerialNum.
* Description    : Create the serial number string descriptor.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Get_SerialNum(void)
{
     u32 Device_Serial0, Device_Serial1, Device_Serial2;
     Device_Serial0 = *(u32*)(0x1FFFF7E8);
     Device_Serial1 = *(u32*)(0x1FFFF7EC);
     Device_Serial2 = *(u32*)(0x1FFFF7F0);
     if (Device_Serial0 != 0)
     {
         CustomHID_StringSerial[2] = (u8)(Device_Serial0 & 0x000000FF);  
	 CustomHID_StringSerial[4] = (u8)((Device_Serial0 & 0x0000FF00) >> 8);
	 CustomHID_StringSerial[6] = (u8)((Device_Serial0 & 0x00FF0000) >> 16);
	 CustomHID_StringSerial[8] = (u8)((Device_Serial0 & 0xFF000000) >> 24);
     
	 CustomHID_StringSerial[10] = (u8)(Device_Serial1 & 0x000000FF);  
	 CustomHID_StringSerial[12] = (u8)((Device_Serial1 & 0x0000FF00) >> 8);
	 CustomHID_StringSerial[14] = (u8)((Device_Serial1 & 0x00FF0000) >> 16);
	 CustomHID_StringSerial[16] = (u8)((Device_Serial1 & 0xFF000000) >> 24);
     
	 CustomHID_StringSerial[18] = (u8)(Device_Serial2 & 0x000000FF);  
	 CustomHID_StringSerial[20] = (u8)((Device_Serial2 & 0x0000FF00) >> 8);
	 CustomHID_StringSerial[22] = (u8)((Device_Serial2 & 0x00FF0000) >> 16);
	 CustomHID_StringSerial[24] = (u8)((Device_Serial2 & 0xFF000000) >> 24);
      }
}
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
