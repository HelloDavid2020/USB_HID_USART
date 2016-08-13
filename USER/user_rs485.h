#ifndef _USER_RS485_H_
#define _USER_RS485_H_

#include "stm32f10x.h"
#include <stdbool.h>


extern u8  rs485_rx_buf[500];
extern u16 rs485_rx_len;
extern void Delay(vu32 nCount);
extern void delay_ms(u32 ms);
extern u16 check_sum(u8* data, int len);
extern bool connect_device(u8 id);
extern u8 scan_device(void);
extern u8 check_device_id(u8 *buf,u8 len);
#endif

