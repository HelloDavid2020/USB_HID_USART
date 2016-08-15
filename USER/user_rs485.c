#include "user_rs485.h"
#include "usart1.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"


u8  rs485_rx_buf[500];
u8  source_add[4]={0};
u8  sa_index=0;

u16 rs485_rx_len=0;

u32 com_time_out =0; // 通信超时处理

BIAOTOU biaotouA;
BIAOTOU biaotouB;
BIAOTOU biaotouC;
BIAOTOU biaotouD;
BIAOTOU biaotou[4];

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

// 处理数据帧
bool handle_data_frame(u8 *buf,u8 len,BIAOTOU *biaotou)
{
	u16 sum=0;
	u8 high=0;
	u8 low=0;

	if(len>9)
	{
		if(buf[0]==0xAA && buf[1]==0x55 && buf[2]==0x06 && buf[3]==0xF6 && buf[4]==0x80 )
		{
			check_sum(buf,len);
			sum = check_sum(buf, len - 2);

			high = (u8)(sum >> 8);
			low = (u8)(sum & 0xFF);
			if ((low == buf[len - 1]) && (high == buf[len - 2]))
			{

				if(biaotou->address==buf[5])
				{
					biaotou->ivalue=*(signed short*)(buf+6);
					
					if(biaotou->type==0x12)
					{
						biaotou->real_value=biaotou->ivalue/100.00;
					}
					if(biaotou->type==0x11)
					{
						biaotou->real_value=biaotou->ivalue/10000.00;
					}
					return true; // 解析成功
				}	
				else
				{
					return false; // sa 不存在
				}
			}
		}
		else
		{
			return false;// 校验失败
		}

	}
	return false;
}
	



// 返回1标识ID存在，返回0表示设备不存在
// 
bool get_device_value(BIAOTOU *biaotou,u8 timeout)
{
	u16 value= 0;
	u8 cmd[30] = {0};
	u16 sum = 0;
	int h = 0;
	int l = 0;
	com_time_out=timeout;

	
	
	memset(rs485_rx_buf,0x00,sizeof rs485_rx_buf);
	rs485_rx_len=0;
	
	cmd[0] = 0xAA;
	cmd[1] = 0x55;
	cmd[2] = 0x04;
	cmd[3] = 0xFE;
	cmd[4] = biaotou->address;
	cmd[5] = 0x80;

	sum = check_sum(cmd, 6);

	h = sum >> 8;
	l = sum & 0xFF;

	cmd[6] = (u8)h;
	cmd[7] = (u8)l;

	rs485_send(cmd,8);
	while(com_time_out)
	{
		if(handle_data_frame(rs485_rx_buf,rs485_rx_len,biaotou)==true)
		{
			return true;
		}
	}
	return false;
}




u8 scan_device(void)
{
	// AA 55 04 F4 sa 80 sH sL
	u8 num = 0;
	u8 sa = 0;
	u8 n = 0;
	BIAOTOU tbiaotou;

	
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
			
				delay_ms(50);
				if(rs485_rx_len>2)
				{
					if(find_device(rs485_rx_buf,rs485_rx_len,&tbiaotou)==true)
					{
						//memcpy(&biaotou[n++],&tbiaotou,sizeof tbiaotou);
						
						biaotou[n].address=tbiaotou.address;
						biaotou[n].sn=tbiaotou.sn;
						biaotou[n].type=tbiaotou.type;
						biaotou[n].ivalue=tbiaotou.ivalue;
						biaotou[n].liangcheng=tbiaotou.liangcheng;
						biaotou[n].real_value=tbiaotou.real_value;
						n++;
						source_add[sa_index++]=tbiaotou.address;
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

bool find_device(u8 *buf,u8 len,BIAOTOU * biaotou)
{
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
			return true;

		}
		else
		{
			return false;
		}

	}
	return false;

}










