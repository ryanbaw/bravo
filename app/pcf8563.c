/************************************************
�ļ���pcf8563.h
��;��pcf8563����ͷ�ļ�
ע�⣺ϵͳʱ��8M
************************************************/
#include "config.h"

uint8 syserr;
uint8 *week_list[7]=
{
	(uint8 *)"Sun",
	(uint8 *)"Mon",
	(uint8 *)"Tue",
	(uint8 *)"Wed",
	(uint8 *)"Thu",
	(uint8 *)"Fri",
	(uint8 *)"Sat"
};

uint8 write_buff[8];
uint8 read_buff[8];
uint8 time_tmp[] = "00:00:00 ";
uint8 data_tmp[] = "2008-01-01 ";
uint8 *week_tmp;

uint8 time[7]=
{
	0x50/*��*/,
	0x59/*��*/,
	0x23/*ʱ*/,
	0x31/*��*/,
	0x02/*����*/,
	0x12/*��/����*/,
	0x10/*��*/
};
/*************************************************************************
** ��������: clear(uint8 *p,uint8 num)
** ��������: ���ָ������ָ�����ȵ�����
** �䡡��: uint8 *p :��ʼ��ַ   uint8 num :�����ݳ���
**************************************************************************/
void clear(uint8 *p,uint8 num)
{
	uint8 *pbuf = (uint8 *)p;
		
	for(; num > 0; num--)
	{
		*pbuf=0;
		pbuf++;
	}
}
/*************************************************************************
** ��������: pcf8536_write(uint8 addr,uint8 data)
** ��������: ��pcf8563ָ����ַд��һ������
** �䡡��: uint8 addr    ���߰�λΪ������ַ���Ͱ�λΪ�ڲ��Ĵ�����ַ
uint8 data  ����Ҫд�������
**************************************************************************/
void pcf8536_write(uint8 addr,uint8 data)
{
	i2c_start();
	
	if(i2c_write_byte(WR_ADDR_CMD)==SLA_W)
	{
		i2c_write_byte(addr);
		i2c_write_byte(data);
	}
	else 
	{
		syserr=ERR_SLA_W;
	}
	
	i2c_stop();
}
/*************************************************************************
** ��������: pcf8536_write_p(uint8 addr,uint8 *p,uint8 num)
** ��������: ��pcf8563��ַ�����ļĴ���д��ϵ������
** �䡡��: uint8 addr    ���߰�λΪ������ַ���Ͱ�λΪ�ڲ��Ĵ�����ַ
uint8 *p    ����Ҫд������������ʼ��ַ
uint8 num   ��д�����ݵĸ���
** ˵����д����������Ϊ��ַ�����ļĴ���
**************************************************************************/
void pcf8536_write_p(uint8 addr,uint8 *p,uint8 num)
{
	uint8 *pbuf = p;
	
	i2c_start();
	if(i2c_write_byte(WR_ADDR_CMD)==SLA_W)
	{
		i2c_write_byte(addr);
		for(;num>0;num--)
		{
			i2c_write_byte(*pbuf);
			pbuf++;
			_NOP();
		}
	}
	else 
	{
		syserr=ERR_SLA_W;
	}
	i2c_stop();
}
/*************************************************************************
** ��������: pcf8536_read(uint8 addr,uint8 *p,uint8 num)
** ��������: ��pcf8563
** �䡡��: uint8 addr    ���߰�λΪ������ַ���Ͱ�λΪ�ڲ��Ĵ�����ַ
uint8 *p    �����������ݴ�ŵ�ַ����ʼ��ַ
uint8 num   ���������ݵĸ���
**************************************************************************/
void pcf8536_read(uint8 addr,uint8 *p,uint8 num)
{
	uint8 *pbuf = p;

	i2c_start();
	if(i2c_write_byte(WR_ADDR_CMD)==SLA_W)
	{
		i2c_write_byte(addr);
	}
	else 
	{
		syserr=ERR_SLA_W;
	}

	i2c_start();
	if(i2c_write_byte(RD_ADDR_CMD)==SLA_W)
	{
		i2c_write_byte(addr);
	}
	else 
	{
		syserr=ERR_SLA_W;
	}

	for(; num > 0; num--)
	{
		*pbuf = i2c_read_byte();
		pbuf++;
	}
}
/*************************************************************************
** ��������: pcf8563_init(void)
** ��������: pcf8563��ʼ��
** �䡡��: 
** ���	 : 
** ȫ�ֱ���: 
** ����ģ��: 
** ˵����
** ע�⣺
**************************************************************************/
void pcf8563_init(void)
{
	clear(write_buff,8);
	clear(read_buff,8);
	pcf8563_stop();
	pcf8563_time_set((uint8 *)time);
	pcf8563_start();
}
/*************************************************************************
** ��������: pcf8563_time_sort(void)
** ��������: ˢ��ʱ��Ĵ����������������
** �䡡��: 
** ���	 : 
** ȫ�ֱ���: 
** ����ģ��: 
** ˵����
** ע�⣺
**************************************************************************/
void pcf8563_time_sort(void)
{
	time[0]=time[0]&0x7F;	//S
	time[1]=time[1]&0x7F;	//M
	time[2]=time[2]&0x3F;	//H
	time[3]=time[3]&0x3F;	//D
	time[4]=time[4]&0x07;	//W
	time[5]=time[5]&0x1F;	//M
	time[6]=time[6]&0xFF;	//Y	
	
	time_tmp[0]=(time[2]>>4)+0x30;
	time_tmp[1]=(time[2]&0x0F)+0x30; 	//H
	time_tmp[3]=(time[1]>>4)+0x30;
	time_tmp[4]=(time[1]&0x0F)+0x30;	//M
	time_tmp[6]=(time[0]>>4)+0x30;
	time_tmp[7]=(time[0]&0x0F)+0x30;	//S
	
	data_tmp[2]=(time[6]>>4)+0x30;	
	data_tmp[3]=(time[6]&0x0F)+0x30;	//Y
	data_tmp[5]=(time[5]>>4)+0x30;
	data_tmp[6]=(time[5]&0x0F)+0x30;	//M
	data_tmp[8]=(time[3]>>4)+0x30;
	data_tmp[9]=(time[3]&0x0F)+0x30;	//D
	
	week_tmp=week_list[time[4]&0x0F];	//W
}

void pcf8563_start(void)
{
	pcf8536_write(PCF8563_CSR1, PCF8563_START);
}

void pcf8563_stop(void)
{
	pcf8536_write(PCF8563_CSR1, PCF8563_STOP);
}

void pcf8563_time_set(uint8 *time_list)
{
	pcf8536_write_p(PCF8563_SEC, time_list, 7);		//pcf8563д��ʱ���е�ַ��������
}

void pcf8563_time_get(uint8 *time_list)
{
	pcf8536_read(PCF8563_SEC, time_list, 7);
}

#if PCF8563_TEST == 1
void pcf8563_test(void)
{	
	uint8 temp = time[0];
	pcf8563_time_get(time);
	
	pcf8563_time_sort();
	
	if(temp != time[0])
	{
		uart_putnstring((uint8 *)data_tmp, 11);
		uart_putnstring((uint8 *)time_tmp, 9);
		uart_putstring((uint8 *)week_tmp);
		uart_putenter();
	}
	#ifndef USE_UCOS
	//OSTimeDly(OS_TICKS_PER_SEC/2); 
	#endif
	
	#ifndef USE_UCOS
	//_delay_ms(1000);
	#endif
}
#endif