#ifndef _USER_GPIO_H_
#define _USER_GPIO_H_

#include "stm32f10x.h"
#include <stdbool.h>


extern void usb_gpio_init(void);
extern void led_gpio_init(void);
extern void GPIO_Toggle(GPIO_TypeDef* GPIOx, vu16 GPIO_Pin);
#endif


