#include "user_rs485.h"
#include "usart1.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"


u8  rs485_rx_buf[500];
u8  source_add[4]={0};
u8  sa_index=0;

u16 rs485_rx_len=0;

BIAOTOU biaotouA;
BIAOTOU biaotouB;
BIAOTOU biaotouC;
BIAOTOU biaotouD;
BIAOTOU *biaotou[4];

void rs485_send(uint8_t *buffer, uint8_t len)
{
	RS485_RX_DIS();
	UART_sendPacket(USART3,buffer,len);		
	RS485_RX_EN();
}




u16 check_sum(u8* data, int len)
{
		u16 i = 0;
		u16 checksum = 0;
		for (i = 2; i < len; i++)
		{
				checksum += data[i];
		}

		return checksum;
}

// 返回1标识ID存在，返回0表示设备不存在
bool connect_device(u8 id)
{
	//byte res = 0;
	u8 cmd[30] = {0};
	u16 sum = 0;
	int h = 0;
	int l = 0;
	cmd[0] = 0xAA;
	cmd[1] = 0x55;
	cmd[2] = 0x04;
	cmd[3] = 0xF4;
	cmd[4] = id;
	cmd[5] = 0x80;

	sum = check_sum(cmd, 6);

	h = sum >> 8;
	l = sum & 0xFF;

	cmd[6] = (u8)h;
	cmd[7] = (u8)l;

	rs485_send(cmd,8);
	
	
	return true;

}

// 返回1标识ID存在，返回0表示设备不存在
u16 get_device_data(u8 id)
{
	u16 value= 0;
	u8 cmd[30] = {0};
	u16 sum = 0;
	int h = 0;
	int l = 0;
	cmd[0] = 0xAA;
	cmd[1] = 0x55;
	cmd[2] = 0x04;
	cmd[3] = 0xF4;
	cmd[4] = id;
	cmd[5] = 0x80;

	sum = check_sum(cmd, 6);

	h = sum >> 8;
	l = sum & 0xFF;

	cmd[6] = (u8)h;
	cmd[7] = (u8)l;

	rs485_send(cmd,8);
	
	
	return true;

}




u8 scan_device(void)
{
	// AA 55 04 F4 sa 80 sH sL
	u8 num = 0;
	u8 sa = 0;
	u8 n = 0;

	
	u8 source_address = 0;
// 先发一次
	connect_device(0);
	rs485_rx_len=0;
	memset(rs485_rx_buf,0x00,sizeof rs485_rx_buf);	
	delay_ms(10);
// --------------------------------------------------------

	for (source_address = 1; source_address <= 100; source_address++)
	{

			connect_device(source_address);
			
				delay_ms(20);
				if(rs485_rx_len>2)
				{
					biaotou[n] = find_device(rs485_rx_buf,rs485_rx_len);
					if(biaotou[n]->address>0)
					{
						n++;
						source_add[sa_index++]=sa;
						rs485_rx_len=0;
						memset(rs485_rx_buf,0x00,sizeof rs485_rx_buf);
					}
					else
					{
					 
					}
				
				
				
				
				num++;
			}
			//System.Threading.Thread.Sleep(100);// 休眠100ms
	}

	return num;

}

BIAOTOU* find_device(u8 *buf,u8 len)
{
	BIAOTOU* biaotou;
	u16 sum=0;
	u8 high=0;
	u8 low=0;

	if(buf[0]==0xAA && buf[1]==0x55)
	{
		check_sum(buf,len);
		sum = check_sum(buf, len - 2);

		high = (u8)(sum >> 8);
		low = (u8)(sum & 0xFF);
		if ((low == buf[len - 1]) && (high == buf[len - 2]))
		{


			biaotou->address=buf[5];  // ID			
			biaotou->liangcheng=buf[6]; // 量程					
			biaotou->type =buf[7]; //类型	
			biaotou->sn = *(u32*)(buf+8);
			//return biaotou;

		}
		else
		{
		
		}

	}
	return biaotou;

}












