#ifndef _USER_RS485_H_
#define _USER_RS485_H_

#include "stm32f10x.h"
#include <stdbool.h>
typedef struct
{
	u8 address;	// 地址
	u8 liangcheng;// 量程
	u8 type; //类型 电压电流	
u16 value;
}BIAOTOU;


#define RANGE_1KHZ 0x7D 
#define RANGE_10KHZ 0x7E 
#define RANGE_100KHZ 0x7F 
#define RANGE_2MOhm 0xA8 
#define RANGE_200KOhm 0xA9 
#define RANGE_20KOhm 0xAA 
#define RANGE_2KOhm 0xAB 
#define RANGE_200Ohm 0xAC 
#define RANGE_1000A 0xAD 
#define RANGE_900A 0xAE 
#define RANGE_800A 0xAF 
#define RANGE_750A 0xB0 
#define RANGE_600A 0xB1 
#define RANGE_500A 0xB2 
#define RANGE_400A 0xB3 
#define RANGE_300A 0xB4 
#define RANGE_100A 0xB5 
#define RANGE_10A 0xB6 
#define RANGE_30A 0xB7 
#define RANGE_40A 0xB8 
#define RANGE_50A 0xB9 
#define RANGE_60A 0xBA 
#define RANGE_75A 0xBB 
#define RANGE_80A 0xBC 
#define RANGE_90A 0xBD 
#define RANGE_20A 0xBE 
#define RANGE_200A 0xBF 
#define RANGE_2V 0xC1 
#define RANGE_20V 0xC2 
#define RANGE_20mV 0xC3 
#define RANGE_200V 0xC4 
#define RANGE_200mV 0xC5 
#define RANGE_4V 0xC6 
#define RANGE_40V 0xC7 
#define RANGE_40mV 0xC8 
#define RANGE_400V 0xC9 
#define RANGE_400mV 0xCA 
#define RANGE_5V 0xCB
#define RANGE_50V 0xCC 
#define RANGE_50mV 0xCD 
#define RANGE_500V 0xCE 
#define RANGE_500mV 0xCF
#define RANGE_6V 0xD0 
#define RANGE_60V 0xD1 
#define RANGE_60mV 0xD2 
#define RANGE_600V 0xD3 
#define RANGE_600mV 0xD4 
#define RANGE_2A 0xD5 
#define RANGE_2mA 0xD6 
#define RANGE_20mA 0xD7 
#define RANGE_200mA 0xD8 
#define RANGE_200uA 0xD9 
#define RANGE_4mA 0xDA 
#define RANGE_40mA 0xDB 
#define RANGE_400mA 0xDC 
#define RANGE_400uA 0xDD 
#define RANGE_5mA 0xDE 
#define RANGE_50mA 0xDF 
#define RANGE_500mA 0xE0 
#define RANGE_500uA 0xE1 
#define RANGE_6mA 0xE2 
#define RANGE_60mA 0xE3 
#define RANGE_600mA 0xE4 
#define RANGE_600uA 0xE5 
#define RANGE_4A 0xE6 
#define RANGE_5A 0xE7 
#define RANGE_6A 0xE8 
#define RANGE_2KV 0xE9 
#define RANGE_NKV 0xEA 
#define RANGE_2mV 0xEB 
#define RANGE_20uA 0xEC 
#define RANGE_2KA 0xED 
#define RANGE_NKA 0xEE 
#define RANGE_700V 0xEF

#define CLASS_DC_FOUR_HALF 0x11 /* 直流四位半表头 */ 
#define CLASS_DC_THREE_HALF 0x12 /* 直流三位半表头 */ 
#define CLASS_AC_FOUR_HALF 0x21 /* 交流四位半表头 */ 
#define CLASS_AC_THREE_HALF 0x22 /* 交流三位半表头 */

extern u8  source_add[4];
extern u8  sa_index;

extern u8  rs485_rx_buf[500];
extern u16 rs485_rx_len;
extern void Delay(vu32 nCount);
extern void delay_ms(u32 ms);
extern u16 check_sum(u8* data, int len);
extern bool connect_device(u8 id);
extern u8 scan_device(void);
extern u8 check_device_id(u8 *buf,u8 len);
#endif

